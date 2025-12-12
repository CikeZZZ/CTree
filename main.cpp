#include "Common.h"
#include "Lang.h"
#include "Utils.h"
#include "Config.h"
#include "FileGenerator.h"
#include "TreeGenerator.h"
#include "ContextMenu.h"

/// @brief 执行目录树生成或文件重建（核心主逻辑入口）
/// @param cfg 已解析的配置
/// 
/// @note 该函数根据 cfg.isRebuildMode 自动分发：
///       - false: 生成目录树（inputPath 是目录）
///       - true:  重建文件结构（inputPath 是文本文件）
void RunTreeGeneration(const AppConfig& cfg) {
    // 1. 检查输入路径是否存在
    if (!fs::exists(cfg.inputPath)) {
        std::cout << to_utf8(Strings::get("ERR_PATH")) << std::endl;
        return;
    }

    // 2. ========== 重建模式 (Rebuild) ==========
    if (cfg.isRebuildMode) {
        // Case 5: ctree -i tree.txt (无 -o) -> 报错
        // Case 6: ctree -i tree.txt -o out/ -> 正常
        if (cfg.outputPath.empty()) {
            // 这里你可以新增一个语言资源 KEY，或者直接输出错误
            std::cerr << "Error: Rebuild mode requires an output directory (-o)." << std::endl;
            return;
        }

        // 执行重建
        RebuildTreeFromFile(cfg.inputPath, cfg.outputPath, cfg.emptyFolder);
        return;
    }

    // 3. ========== 生成模式 (Generate) ==========

    // 加载忽略规则 (逻辑保持不变)
    TreeIgnore ignoreMgr;
    bool loaded = false;
    if (!cfg.specifiedIgnoreFile.empty()) {
        ignoreMgr.load_file(cfg.specifiedIgnoreFile);
        loaded = true;
    }
    if (!loaded) {
        fs::path localIgnore = cfg.inputPath / IGNORE_FILENAME;
        fs::path cwdIgnore = fs::current_path() / IGNORE_FILENAME;
        fs::path globalIgnore = get_global_ignore_path();

        if (fs::exists(localIgnore)) ignoreMgr.load_file(localIgnore);
        else if (fs::exists(cwdIgnore)) ignoreMgr.load_file(cwdIgnore);
        else ignoreMgr.load_file(globalIgnore);
    }
    for (const auto& r : cfg.tempIgnores) ignoreMgr.add_rule(r);
    ignoreMgr.set_root(cfg.inputPath);
    if (!cfg.outputPath.empty()) ignoreMgr.add_rule(cfg.outputPath.filename().wstring());

    // 4. 判定输出模式
    // Case 1 & 2: -i src (无 -o) -> 输出到 Console
    // Case 3 & 4: -i src -o ...  -> 输出到 File
    // 假设 AppConfig 有一个标志 cfg.saveToFile 表示用户敲了 -o
    // 如果没有这个标志，根据 outputPath 是否为空来判断也可以（但无法区分 Case 3）
    // 这里我们假设：如果 outputPath 为空 且 saveToFile 为 false -> ConsoleMode

    bool consoleMode = cfg.outputPath.empty() && !cfg.OutputFlag;

    // 剪贴板逻辑 (仅 Text 格式且开启复制时)
    std::wstringstream clipboardBuffer;
    std::wstringstream* pClipPtr = nullptr;
    if (cfg.CopyFlag && cfg.format == OutputFormat::TEXT) {
        pClipPtr = &clipboardBuffer;
    }

    // 5. 执行生成
    // 注意：GenerateTree 签名已修改，增加了 consoleMode 参数
    GenerateTree(cfg.inputPath, cfg.outputPath, ignoreMgr, cfg.format, pClipPtr, consoleMode);

    // 6. 写入剪贴板
    if (pClipPtr) {
        CopyToClipboardW(clipboardBuffer.str());
    }
}

/// @brief 从普通文件读取内容并复制到剪贴板（处理 `-c file.txt` 且无 `-i` 的情况）
/// @param filePath 要读取的文件路径
void RunFileContentCopy(const fs::path& filePath) {
    if (!fs::exists(filePath)) {
        std::cout << to_utf8(Strings::get("ERR_PATH")) << std::endl;
        return;
    }
    std::ifstream in(filePath, std::ios::binary);
    if (!in.is_open()) {
        std::cout << to_utf8(Strings::get("ERR_FILE_READ") + filePath.wstring()) << std::endl;
        return;
    }

    // 自动跳过 UTF-8 BOM
    char bom[3] = { 0 };
    in.read(bom, 3);
    if (!(bom[0] == '\xEF' && bom[1] == '\xBB' && bom[2] == '\xBF')) {
        in.seekg(0);
    }

    // 读取全部内容
    std::ostringstream ss;
    ss << in.rdbuf();

    // 复制到剪贴板（自动处理 UTF-8 → WideChar）
    CopyToClipboard(ss.str());
}

/// @brief 程序主入口（宽字符版本，支持 Unicode 命令行参数）
int wmain(int argc, wchar_t* argv[]) {
    // 启用控制台 UTF-8 支持（输出/输入）
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    // 解析命令行参数
    AppConfig config;
    config.parse(argc, argv);

    // --- 基础信息与帮助 ---
    if (config.showHelp) {
        std::cout << to_utf8(Strings::get("HELP_MSG"));
        return 0;
    }
    if (config.showVersion) {
        std::cout << to_utf8(VERSION);
        return 0;
    }

    // --- 参数校验失败 ---
    if (!config.isValid) {
        // 具体错误已在 parse 中输出，这里统一提示
        std::cout << to_utf8(Strings::get("ERR_Args")) << std::endl;
        return 1;
    }

    // --- 交互式菜单模式（无参数启动）---
    if (config.showMenu) {
        ShowInteractiveMenu();
        return 0;
    }

    // --- 忽略文件管理命令 ---
    if (config.createGlobal) {
        create_ignore_template(get_global_ignore_path());
    }
    if (config.deleteGlobal) {
        fs::path p = get_global_ignore_path();
        if (fs::exists(p)) {
            fs::remove(p);
            std::cout << to_utf8(Strings::get("SUCCESS_DEL_GLOBAL")) << std::endl;
        }
        else {
            std::cout << to_utf8(Strings::get("INFO_REM_GLOBAL")) << std::endl;
        }
    }
    if (config.createLocal) {
        fs::path targetDir = config.inputPath.empty() ? fs::current_path() : config.inputPath;
        create_ignore_template(targetDir / IGNORE_FILENAME);
    }

    // --- 核心业务分发 ---
    // 注意：现在 **所有带 -i 的操作都走 RunTreeGeneration**
    //       它内部会根据 inputPath 类型自动选择“生成”或“重建”

    if (config.CopyFlag && !config.copyFilePath.empty() && config.inputPath.empty()) {
        // 情况1: 仅复制文件内容（如 treegen -c readme.txt）
        RunFileContentCopy(config.copyFilePath);
    }
    else if (!config.inputPath.empty()) {
        // 情况2: 有 -i 参数 → 智能分发
        RunTreeGeneration(config);
    }
    // 若既无 -i 也无 -c，则无操作（但通常会被 showMenu 捕获）

    return 0;
}