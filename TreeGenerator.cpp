#include "TreeGenerator.h"
#include "Lang.h"
#include "Utils.h"
#include <cstdio> // 用于 snprintf 格式化输出

/// @brief 行解析结果结构体
struct LineParsingResult {
    int level;          ///< 缩进层级（0 = 根节点）
    std::wstring name;  ///< 净文件/目录名（不含前缀和末尾斜杠）
    bool isDir;         ///< 是否为目录（原行以 \ 或 / 结尾）
    bool isValid;       ///< 解析是否成功（非空、格式合法）
};

/// @brief 健壮地解析一行树状文本（如 "├── src" 或 "│   └── main.cpp"）
/// @param lineRaw 原始输入行（UTF-8 转来的宽字符串）
/// @return 解析结果，若无效则 isValid = false
LineParsingResult ParseLineRobust(const std::wstring& lineRaw) {
    LineParsingResult res = { 0, L"", false, true };

    // 空行直接无效
    if (lineRaw.empty()) {
        res.isValid = false;
        return res;
    }

    // 去除右侧空白（空格、制表符、换行等），避免文件名带空格导致创建失败
    std::wstring line = lineRaw;
    const wchar_t* ws = L" \t\n\r";
    size_t lastChar = line.find_last_not_of(ws);
    if (lastChar != std::wstring::npos) {
        line.erase(lastChar + 1);
    }
    else {
        res.isValid = false; // 全是空白字符
        return res;
    }

    // CTree 风格的标准前缀块（4字符一组）
    const std::vector<std::wstring> prefixes = { L"├── ", L"└── ", L"│   ", L"    " };

    size_t pos = 0;
    // 逐个匹配前缀块，每匹配一个层级 +1
    while (pos + 4 <= line.size()) {
        std::wstring chunk = line.substr(pos, 4);
        bool match = false;
        for (const auto& p : prefixes) {
            if (chunk == p) {
                res.level++;
                pos += 4;
                match = true;
                break;
            }
        }
        if (!match) break; // 遇到非前缀内容，停止解析层级
    }

    // 提取实际名称部分
    res.name = line.substr(pos);
    if (res.name.empty()) {
        res.isValid = false;
        return res;
    }

    // 判断是否为目录：末尾有 \ 或 /
    if (res.name.back() == L'\\' || res.name.back() == L'/') {
        res.isDir = true;
        res.name.pop_back(); // 去掉斜杠，保留干净名称
    }

    return res;
}

/// @brief 从树状文件重建真实目录/文件结构
/// @param treeFile 输入的树状文本文件路径（如 tree.txt）
/// @param destRoot 目标根目录（所有内容将在此下重建）
/// @param emptyFolder 是否在开始前清空目标目录
void RebuildTreeFromFile(const fs::path& treeFile, const fs::path& destRoot, bool emptyFolder) {
    // 1. 输入文件存在性检查
    if (!fs::exists(treeFile)) {
        std::cerr << to_utf8(Strings::get("ERR_FILE_OPEN") + treeFile.wstring()) << std::endl;
        return;
    }

    // 2. 可选：清空目标目录
    if (emptyFolder) {
        if (fs::exists(destRoot) && !fs::is_empty(destRoot)) {
            std::cout << to_utf8(Strings::get("MSG_EMPTY") + destRoot.wstring()) << std::endl;
            std::error_code cleanEc;
            fs::remove_all(destRoot, cleanEc); // 递归删除整个目录（包括 destRoot 自身！）
            if (cleanEc) {
                std::cerr << "Warning: Clean failed: " << cleanEc.message() << std::endl;
            }
        }
    }

    // 3. 重新创建根目录（因为 remove_all 会删掉 destRoot 本身）
    std::error_code ec;
    fs::create_directory(destRoot, ec);
    if (ec) {
        std::cerr << "Error recreating root: " << ec.message() << std::endl;
        return;
    }

    // 4. 打开树状文件（支持 UTF-8）
    std::ifstream file(treeFile, std::ios::binary);
    if (!file.is_open()) return;

    // 5. BOM 处理：跳过 UTF-8 BOM（EF BB BF）
    char bom[3] = { 0 };
    file.read(bom, 3);
    if (!(bom[0] == '\xEF' && bom[1] == '\xBB' && bom[2] == '\xBF')) {
        file.seekg(0); // 无 BOM，重置到开头
    }

    // 6. 初始化路径栈：stack[i] 表示第 i 层当前所在的物理路径
    //    stack[0] = destRoot（根目录）
    std::vector<fs::path> pathStack;
    pathStack.reserve(20); // 预留空间，避免频繁 realloc
    pathStack.push_back(destRoot);

    std::cout << to_utf8(Strings::get("PROCESSING")) << std::endl;

    // 7. 统计信息
    int lineNum = 0;
    int countSuccess = 0; // 成功创建
    int countSkip = 0;    // 已存在，跳过
    int countError = 0;   // 创建失败
    char logBuf[256];     // 用于格式化输出

    // 8. 逐行处理
    std::string lineUTF8;
    while (std::getline(file, lineUTF8)) {
        // 处理 Windows 换行符 \r\n
        if (!lineUTF8.empty() && lineUTF8.back() == '\r')
            lineUTF8.pop_back();
        if (lineUTF8.empty()) continue;

        lineNum++;
        LineParsingResult info = ParseLineRobust(to_wide(lineUTF8));
        if (!info.isValid) continue;

        // --- 情况 A：根节点行（level == 0）---
        // 通常格式如 "MyProject\"，但我们**不改变 destRoot 名称**
        // 所有内容都直接生成在 -o 指定的目录下（保持用户指定结构）
        if (info.level == 0) {
            // 重置栈，防止文件中出现多个根导致路径错乱
            pathStack.clear();
            pathStack.push_back(destRoot);
            continue; // 忽略根节点名称
        }

        // --- 情况 B：普通节点（level >= 1）---
        size_t parentStackIdx = static_cast<size_t>(info.level - 1);

        // [健壮性检查 1]：缩进层级跳跃过大（如栈只有 2 层，却来了 level=5 的行）
        if (parentStackIdx >= pathStack.size()) {
            snprintf(logBuf, sizeof(logBuf),
                to_utf8(Strings::get("ERR_TREE_BROKEN")).c_str(),
                lineNum, (int)pathStack.size(), info.level
            );
            std::cout << logBuf << std::endl;
            countError++;
            continue;
        }

        // [栈调整]：若当前层级比之前浅（如从 level=5 回到 level=2）
        // 则弹出栈中多余路径，只保留到父级
        pathStack.resize(parentStackIdx + 1);

        // 构造完整路径
        fs::path parentPath = pathStack.back();
        fs::path fullPath = parentPath / info.name;

        if (info.isDir) {
            // --- 处理目录 ---
            if (fs::exists(fullPath)) {
                // 目录已存在：不覆盖，但必须“进入”以便处理子项
                pathStack.push_back(fullPath);
                countSkip++; // 算作跳过（未创建）
            }
            else {
                // 尝试创建新目录
                fs::create_directory(fullPath, ec);
                if (ec) {
                    std::cerr << "Error: " << ec.message()
                        << " [" << to_utf8(info.name) << "]" << std::endl;
                    countError++;
                    // 创建失败则不入栈，后续子项会因“缩进断裂”被跳过
                }
                else {
                    pathStack.push_back(fullPath);
                    std::cout << to_utf8(Strings::get("MSG_CREATE_DIR") + fullPath.wstring()) << std::endl;
                    countSuccess++;
                }
            }
        }
        else {
            // --- 处理文件 ---
            if (fs::exists(fullPath)) {
                std::cout << to_utf8(Strings::get("MSG_SKIP_EXIST") + fullPath.wstring()) << std::endl;
                countSkip++;
            }
            else {
                // 创建空文件
                std::ofstream outfile(fullPath);
                if (outfile) {
                    std::cout << to_utf8(Strings::get("MSG_CREATE_FILE") + fullPath.wstring()) << std::endl;
                    countSuccess++;
                }
                else {
                    std::cerr << "Error creating file: " << to_utf8(fullPath.wstring()) << std::endl;
                    countError++;
                }
            }
        }
    }

    // 9. 输出总结报告
    snprintf(logBuf, sizeof(logBuf),
        to_utf8(Strings::get("SUMMARY_DONE")).c_str(),
        countSuccess, countSkip, countError
    );
    std::cout << "-------------------------\n" << logBuf << std::endl;
}