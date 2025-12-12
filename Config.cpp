#include "Config.h"
#include "Lang.h"
#include "Utils.h"

void AppConfig::parse(int argc, wchar_t* argv[]) {
    if (argc < 2) {
        showMenu = true;
        return;
    }

    for (int i = 1; i < argc; ++i) {
        std::wstring arg = argv[i];

        if (arg == L"-h" || arg == L"--help") {
            showHelp = true;
            return;
        }
        if (arg == L"-v" || arg == L"--version") {
            showVersion = true;
            return;
        }

        // 输入路径：可以是目录（生成树）或文件（重建结构）
        if (arg == L"-i" || arg == L"--input") {
            if (i + 1 < argc) {
                inputPath = argv[++i];
            }
            else {
                isValid = false;
            }
        }
        // 输出路径（用于生成树 或 重建目标目录）
        else if (arg == L"-o" || arg == L"--output") {
            OutputFlag = true;
            if (i + 1 < argc && argv[i + 1][0] != L'-') {
                outputPath = argv[++i];
                // 自动检测后缀以支持 --html
                std::wstring ext = outputPath.extension().wstring();
                std::transform(ext.begin(), ext.end(), ext.begin(), ::towlower);
                if (ext == L".html" || ext == L".htm") {
                    format = OutputFormat::HTML;
                }
            }
        }
        // 剪贴板复制
        else if (arg == L"-c" || arg == L"--copy") {
            CopyFlag = true;
            if (i + 1 < argc && argv[i + 1][0] != L'-') {
                copyFilePath = argv[++i];
            }
        }
        // 新增：--empty 仍可用于重建时清空目标目录
        else if (arg == L"--empty") {
            emptyFolder = true;
        }
        // 忽略规则相关
        else if (arg == L"-n" || arg == L"--ignore") {
            while (i + 1 < argc) {
                std::wstring next = argv[i + 1];
                if (!next.empty() && next[0] == L'-') break;
                tempIgnores.push_back(next);
                i++;
            }
        }
        else if (arg == L"-f" || arg == L"--file") {
            if (i + 1 < argc) {
                specifiedIgnoreFile = argv[++i];
            }
        }
        else if (arg == L"-g" || arg == L"--global") {
            createGlobal = true;
        }
        else if (arg == L"-d" || arg == L"--delete-global") {
            deleteGlobal = true;
        }
        else if (arg == L"-l" || arg == L"--local") {
            createLocal = true;
        }
        // 其他未知参数
    }

    // --- 路径标准化 ---
    std::error_code ec;
    if (!inputPath.empty()) {
        fs::path abs = fs::absolute(inputPath, ec);
        if (!ec) inputPath = abs;
    }
    if (OutputFlag && !outputPath.empty()) {
        fs::path abs = fs::absolute(outputPath, ec);
        if (!ec) outputPath = abs;
    }

    // --- 自动判断操作模式 ---
    if (!inputPath.empty()) {
        // 检查 -i 指向的是文件还是目录
        if (fs::is_regular_file(inputPath)) {
            // ✅ 是文件 → 进入 **重建模式**
            isRebuildMode = true;

            // 重建必须指定输出目录
            if (!OutputFlag || outputPath.empty()) {
                std::cerr << to_utf8(Strings::get("ERR_REBUILD_NO_OUT")) << std::endl;
                isValid = false;
            }
        }
        else if (fs::is_directory(inputPath)) {
            // ✅ 是目录 → 进入 **生成树模式**
            isRebuildMode = false;
            // -o 可选（可输出到控制台、剪贴板等）
        }
        else {
            // 路径不存在或类型未知
            std::cerr << to_utf8(Strings::get("ERR_PATH") + inputPath.wstring()) << std::endl;
            isValid = false;
        }
    }

    // --- 互斥校验（其实已隐含，但保留以防万一）---

}