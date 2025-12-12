#include "TreeIgnore.h"
#include "Utils.h"
#include "Lang.h"

/// @brief 设置忽略规则的根目录路径
/// @param path 目录的绝对路径，用于计算相对路径进行匹配
void TreeIgnore::set_root(const fs::path& path) {
    rootPath = path;
}

/// @brief 向忽略规则列表中添加一条原始规则（来自 .treeignore 文件的一行）
/// @param raw 原始规则字符串（可能包含空格、注释、通配符等）
/// @note 会自动处理：  
///       - 去除前后空白  
///       - 转换 '/' 为 Windows 风格 '\'  
///       - 跳过注释行（#开头）  
///       - 处理以 \ 开头的“仅根目录”规则  
///       - 处理以 \\ 开头的非法规则（直接跳过）  
///       - 移除开头的 .\（表示当前目录，无实际意义）  
///       - 识别末尾 \ 表示“仅目录”  
void TreeIgnore::add_rule(std::wstring raw) {
    const wchar_t* ws = L" \t\n\r"; // 空白字符
    size_t start = raw.find_first_not_of(ws);
    if (start == std::wstring::npos) return; // 全是空白，跳过
    raw.erase(0, start);
    size_t end = raw.find_last_not_of(ws);
    if (end != std::wstring::npos) raw.erase(end + 1);
    if (raw.empty()) return;

    // 统一路径分隔符为反斜杠（Windows 风格）
    std::replace(raw.begin(), raw.end(), L'/', L'\\');

    // 跳过注释行
    if (raw[0] == L'#') return;
    // 跳过包含双反斜杠的非法规则（如 "\\folder"）
    if (raw.find(L"\\\\") != std::wstring::npos) return;

    // 标记：是否为“仅在根目录匹配”的规则（以 \ 开头）
    bool isRootOnly = false;
    if (raw[0] == L'\\') {
        isRootOnly = true;
        raw.erase(0, 1); /*去掉开头的 \*/
    }

    // 移除开头的 ".\\"（例如 ".\\build" → "build"），这些在相对路径中无意义
    while (raw.size() >= 2 && raw[0] == L'.' && raw[1] == L'\\') {
        raw.erase(0, 2);
    }
    if (raw.empty()) return;

    // 标记：是否仅匹配目录（规则末尾有 \）
    bool onlyDir = false;
    if (!raw.empty() && raw.back() == L'\\') {
        onlyDir = true;
        raw.pop_back(); // 去掉末尾的 \
                            if (raw.empty()) return; // 如果只剩一个反斜杠，无效规则
    }

    // 判断规则中是否包含路径分隔符（即是否是子路径）
    bool hasSeparator = (raw.find(L'\\') != std::wstring::npos);

    // 保存解析后的规则
    rules.push_back({ raw, onlyDir, isRootOnly, hasSeparator });
}

/// @brief 从指定的 .treeignore 文件加载忽略规则
/// @param path 文件路径（如 .treeignore）
/// @note 会自动检测并跳过 UTF-8 BOM，按行读取并调用 add_rule
void TreeIgnore::load_file(const fs::path& path) {
    if (!fs::exists(path)) return;

    // 输出提示信息：正在使用该 ignore 文件
    std::cout << to_utf8(Strings::get("USING_IGNORE") + path.wstring()) << '\n';

    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) return;

    // 检测 UTF-8 BOM（EF BB BF）
    char bom[3] = { 0 };
    file.read(bom, 3);
    if (!(bom[0] == '\xEF' && bom[1] == '\xBB' && bom[2] == '\xBF')) {
        file.seekg(0); // 无 BOM，回到文件开头
    }

    std::string line;
    while (std::getline(file, line)) {
        // 处理 Windows 换行符（\r\n）中的 \r
        if (!line.empty() && line.back() == '\r') line.pop_back();
        // 转为宽字符串并添加规则
        add_rule(to_wide(line));
    }
}

/// @brief 判断给定路径是否应被忽略
/// @param fullPath 文件或目录的完整路径
/// @param isDirectory 是否是目录
/// @return 若匹配任意忽略规则，返回 true；否则 false
bool TreeIgnore::should_ignore(const fs::path& fullPath, bool isDirectory) const {
    // 计算相对于根目录的路径（用于匹配带路径的规则）
    std::wstring relPathStr;
    if (!rootPath.empty()) {
        std::error_code ec;
        fs::path rel = fs::relative(fullPath, rootPath, ec);
        // 若能成功计算相对路径且不是当前目录 "."，则使用相对路径
        if (!ec && rel != L".")
            relPathStr = rel.wstring();
        else
            // 否则只用文件名（例如根目录下的文件）
            relPathStr = fullPath.filename().wstring();
    }
    else {
        // 未设置根目录，直接使用完整路径
        relPathStr = fullPath.wstring();
    }

    // 文件名（不含路径），用于匹配不带分隔符的规则（如 *.log）
    std::wstring filenameStr = fullPath.filename().wstring();

    // 遍历所有忽略规则
    for (const auto& rule : rules) {
        // 如果规则只匹配目录，但当前是文件，则跳过
        if (rule.onlyDir && !isDirectory) continue;

        if (rule.isRootOnly) {
            // 仅在根目录匹配：直接用相对路径匹配规则（如 \build）
            if (PathMatchSpecW(relPathStr.c_str(), rule.pattern.c_str()))
                return true;
        }
        else if (rule.hasSeparator) {
            // 规则包含路径分隔符（如 src\*.cpp）
            // 1. 在当前层级匹配（如 src\main.cpp）
            if (PathMatchSpecW(relPathStr.c_str(), rule.pattern.c_str()))
                return true;
            // 2. 在任意子目录匹配（如 */src/*.cpp）→ 构造 "*\\pattern"
            std::wstring deepPattern = L"*\\\\" + rule.pattern;
            if (PathMatchSpecW(relPathStr.c_str(), deepPattern.c_str()))
                return true;
        }
        else {
            // 规则不包含路径分隔符（如 *.tmp），只匹配文件名
            if (PathMatchSpecW(filenameStr.c_str(), rule.pattern.c_str()))
                return true;
        }
    }
    return false;
}

/// @brief 获取全局忽略文件的路径（位于用户家目录下）
/// @return 完整路径，如 C:\Users\YourName\.treeignore
fs::path get_global_ignore_path() {
    wchar_t path[MAX_PATH];
    // 获取用户配置目录（CSIDL_PROFILE ≈ %USERPROFILE%）
    if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_PROFILE, NULL, 0, path)))
        return fs::path(path) / IGNORE_FILENAME; // IGNORE_FILENAME 应为 L".treeignore"
    return {};
}

/// @brief 创建默认的 .treeignore 模板文件
/// @param path 要创建的文件路径
/// @note 文件以 UTF-8 BOM 开头，并写入本地化默认内容
void create_ignore_template(const fs::path& path) {
    std::ofstream out(path);
    if (out.is_open()) {
        // 写入 UTF-8 BOM（确保编辑器识别为 UTF-8）
        out << "\xEF\xBB\xBF";
        // 写入本地化默认模板内容（如忽略 build/、*.log 等）
        out << to_utf8(Strings::get("DEFAULT_TREEIGNORE"));
        out.close();

        // 输出成功提示
        std::cout << to_utf8(Strings::get("GENERATED_TREEIGNORE"))
            << to_utf8(path.wstring()) << std::endl;
    }
}