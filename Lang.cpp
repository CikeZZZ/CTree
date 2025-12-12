#include "Lang.h"

Lang detect_system_language() {
    LANGID langId = GetUserDefaultUILanguage();
    // 检测简体中文（包括 LANG_CHINESE 和 LANG_CHINESE_SIMPLIFIED）
    if ((langId & 0xFF) == LANG_CHINESE)
        return Lang::CN;
    return Lang::EN;
}

std::wstring Strings::get(const std::string& key) {
    static Lang currentLang = detect_system_language();
    static const std::map<std::string, std::pair<std::wstring, std::wstring>> dict = {
        // =============== 上下文菜单 ===============
        {"MENU_TITLE", {L"[CTree 安装/卸载 - 当前用户]", L"[CTree Install/Uninstall - Current User]"}},
        {"MENU_OPT_1", {L"1. 添加右键菜单（生成树 + 复制内容）", L"1. Add to Context Menu (Folder Tree + Text Copy)"}},
        {"MENU_OPT_2", {L"2. 从右键菜单移除", L"2. Remove from Context Menu"}},
        {"MENU_OPT_ERROR", {L"输入有误，请重新输入", L"Input is incorrect. Please re-enter."}},
        {"INPUT_PROMPT", {L"请输入选项: ", L"Enter choice: "}},
        {"SUCCESS_ADD", {L"成功！已添加到右键菜单。", L"Success! Added to context menu."}},
        {"SUCCESS_REM", {L"成功！已清理相关右键菜单。", L"Success! Cleaned up context menu."}},
        {"CTX_TREE_NAME", {L"生成目录树", L"Generate Tree File"}},
        {"CTX_COPY_NAME", {L"复制文件内容", L"Copy File Content"}},

        // =============== 通用错误与提示 ===============
        {"ERR_PATH", {L"错误：路径不存在。", L"Error: Path does not exist."}},
        {"ERR_Args", {L"错误：参数不正确。", L"Error: Invalid arguments."}},
        {"ERR_FILE_READ", {L"错误：无法读取文件：", L"Error: Cannot read file: "}},
        {"ERR_FILE_OPEN", {L"错误：无法打开文件：", L"Error: Cannot open file: "}},
        {"MSG_CLIPBOARD", {L"内容已复制到剪贴板。", L"Content copied to clipboard."}},
        {"MSG_SAVED", {L"文件已保存至: ", L"File saved to: "}},
        {"PROCESSING", {L"正在处理...", L"Processing..."}},

        // =============== 重建功能（由 -i 文件触发）==============
        {"MSG_SKIP_EXIST", {L"跳过 (已存在): ", L"Skipped (Exists): "}},
        {"MSG_CREATE_DIR", {L"创建目录: ", L"Created Dir: "}},
        {"MSG_CREATE_FILE", {L"创建文件: ", L"Created File: "}},
        {"MSG_EMPTY", {L"清空目录: ", L"Empty the directory: "}},
        {"ERR_TREE_BROKEN", {L"警告 [行号 %d]: 缩进层级断裂 (期望 <= %d, 实际 %d)，已跳过。", L"Warning [Line %d]: Indentation broken (expected <= %d, got %d). Skipped."}},
        {"ERR_UNK_PREFIX", {L"警告 [行号 %d]: 无法解析的前缀，视为普通文件名。", L"Warning [Line %d]: Unknown prefix, treating as flat filename."}},
        {"ERR_REBUILD_NO_OUT", {L"错误：使用重建模式时，必须指定 -o <path> 输出目录。", L"Error: When using the reconstruction mode, you must specify -o <path> as the output directory."}},
        {"SUMMARY_DONE", {L"操作完成。成功: %d, 跳过: %d, 错误: %d", L"Done. Success: %d, Skipped: %d, Errors: %d"}},

        // =============== 忽略文件管理 ===============
        {"USING_IGNORE", {L"忽略配置文件：", L"Ignore config file: "}},
        {"GENERATED_TREEIGNORE", {L"已创建：", L"Created: "}},
        {"INFO_REM_GLOBAL", {L"全局 .treeignore 不存在", L"Global .treeignore not found"}},
        {"SUCCESS_DEL_GLOBAL", {L"已删除全局 .treeignore", L"Removed global .treeignore"}},

        // =============== 帮助与配置模板 ===============
        {"HELP_MSG", {
            // 中文版
            L"用法: CTree [命令] [参数]\n"
            L"  -h, --help                 显示此帮助消息\n"
            L"  -v, --version              显示软件版本\n"
            L"  -i, --input <path>         指定输入路径（目录则生成树，文件则重建结构）\n"
            L"  -o, --output [path]        1. 输出到文件（可指定路径；若省略，生成带时间戳的文件）\n"
            L"                             2. 若未使用 -o，则输出到控制台\n"
            L"                             3. 重建模式必须指定目录\n"
            L"  -c, --copy [path]          1. 配合 -i 使用：不指定 [path] 时，将生成的目录树复制到剪贴板\n"
            L"                             2. 指定 [path] 时：将该文件内容复制到剪贴板\n"
            L"  -n, --ignore [pattern] ... 临时添加忽略规则（可多次使用）\n"
            L"  -f, --file <path>          使用指定的忽略配置文件（兼容 .gitignore 语法）\n"
            L"  -g, --global               创建全局 .treeignore 配置文件\n"
            L"  -l, --local                在当前目录创建本地 .treeignore 配置文件\n"
            L"  -d, --delete-global        删除全局 .treeignore 配置文件\n"
            L"忽略配置文件优先级：指定 (-f) > 本地 > 全局\n",
            // 英文版
            L"Usage: CTree [command] [args]\n"
            L"  -h, --help                 Display this help message\n"
            L"  -v, --version              Show software version\n"
            L"  -i, --input <path>         Specify input path (directory → generate tree, file → rebuild structure)\n"
            L"  -o, --output [path]        1. Output to file (optional path; if omitted, a timestamped filename is generated)\n"
            L"                             2. If -o is not specified, output to terminal\n"
            L"                             3. The reconstruction mode must specify the directory.\n"
            L"  -c, --copy [path]          1. With -i: omit [path] to copy the generated tree to clipboard\n"
            L"                             2. If [path] is provided, copy its content to clipboard\n"
            L"  -n, --ignore [pattern] ... Add temporary ignore patterns (can be used multiple times)\n"
            L"  -f, --file <path>          Use an explicit ignore config file (compatible with .gitignore syntax)\n"
            L"  -g, --global               Create global .treeignore config file\n"
            L"  -l, --local                Create local .treeignore config file in current directory\n"
            L"  -d, --delete-global        Delete global .treeignore config file\n"
            L"Ignore config priority: explicit (-f) > local > global\n"
        }},
    {"DEFAULT_TREEIGNORE", {
            // 中文版
            L"# ========================================================\n"
            L"# CTree 忽略规则配置文件\n"
            L"# ========================================================\n"
            L"# 语法说明 (基于 .gitignore 逻辑):\n"
            L"#\n"
            L"#【注】本软件内部统一处理路径分隔符，/ 与 \\ 完全等价。\n"
            L"#\n"
            L"# 1. 注释与无效行\n"
            L"#    - 以 # 开头的行会被忽略。\n"
            L"#    - 存在连续斜杠（如 //、\\\\、/\\、\\/）的行会被视为无效而忽略。\n"
            L"#\n"
            L"# 2. 根目录锚定 (以 / 或 \\ 开头)\n"
            L"#    例如: /build\n"
            L"#    说明: 仅匹配项目根目录下的 build 文件夹。\n"
            L"#          (不匹配 sub/build)\n"
            L"#\n"
            L"# 3. 嵌套路径匹配 (中间包含 / 或 \\)\n"
            L"#    例如: src/temp\n"
            L"#    说明: 匹配任意深度下的 src/temp 路径。\n"
            L"#          (匹配 src/temp，也匹配 code/src/temp)\n"
            L"#\n"
            L"# 4. 目录限定 (以 / 或 \\ 结尾)\n"
            L"#    例如: log/\n"
            L"#    说明: 匹配任意位置名为 log 的目录，但不匹配名为 log 的文件。\n"
            L"#\n"
            L"# 5. 文件名/通用匹配 (不含路径分隔符)\n"
            L"#    例如: *.log 或 debug\n"
            L"#    说明: 匹配任意位置符合该名称的文件或目录。\n"
            L"#\n"
            L"# ========================================================\n"
            L"\n"
            L"# --- 版本控制 ---\n"
            L".git/\n"
            L".svn/\n"
            L".hg/\n"
            L"\n"
            L"# --- IDE 与 编辑器 ---\n"
            L".vscode/\n"
            L".idea/\n"
            L".vs/\n"
            L"*.swp\n"
            L"\n"
            L"# --- 常用构建与输出目录 (建议锚定到根目录) ---\n"
            L"/build/\n"
            L"/dist/\n"
            L"/bin/\n"
            L"/obj/\n"
            L"/out/\n"
            L"/target/\n"
            L"\n"
            L"# --- 语言特定依赖 ---\n"
            L"node_modules/\n"
            L"__pycache__/\n"
            L"venv/\n"
            L"\n"
            L"# --- 临时文件与二进制 ---\n"
            L"*.log\n"
            L"*.tmp\n"
            L"*.exe\n"
            L"*.dll\n"
            L"*.so\n"
            L"*.dylib\n"
            L"*.o\n"
            L"*.obj\n"
            L"Thumbs.db\n"
            L".DS_Store\n",
            // 英文版（请确保此处是完整内容，非占位符）
            L"# ========================================================\n"
            L"# CTree Ignore Rules Configuration\n"
            L"# ========================================================\n"
            L"# Syntax Guide (Based on .gitignore logic):\n"
            L"#\n"
            L"# NOTE: This tool internally normalizes path separators; / and \\ are equivalent.\n"
            L"#\n"
            L"# 1. Comments and Invalid Lines\n"
            L"#    - Lines starting with # are ignored.\n"
            L"#    - Lines containing consecutive slashes (e.g., //, \\\\, /\\, \\/) are invalid and skipped.\n"
            L"#\n"
            L"# 2. Root Anchoring (lines starting with / or \\)\n"
            L"#    Example: /build\n"
            L"#    Matches only 'build' in the project root (not sub/build).\n"
            L"#\n"
            L"# 3. Nested Path Matching (contains / or \\ in the middle)\n"
            L"#    Example: src/temp\n"
            L"#    Matches src/temp at any depth (e.g., src/temp, code/src/temp).\n"
            L"#\n"
            L"# 4. Directory-Only (ending with / or \\)\n"
            L"#    Example: log/\n"
            L"#    Matches directories named 'log', but not files.\n"
            L"#\n"
            L"# 5. Filename/General Matching (no path separator)\n"
            L"#    Example: *.log or debug\n"
            L"#    Matches any file or directory with that name anywhere.\n"
            L"#\n"
            L"# ========================================================\n"
            L"\n"
            L"# --- Version Control ---\n"
            L".git/\n"
            L".svn/\n"
            L".hg/\n"
            L"\n"
            L"# --- IDEs & Editors ---\n"
            L".vscode/\n"
            L".idea/\n"
            L".vs/\n"
            L"*.swp\n"
            L"\n"
            L"# --- Common Build & Output Directories (root-anchored recommended) ---\n"
            L"/build/\n"
            L"/dist/\n"
            L"/bin/\n"
            L"/obj/\n"
            L"/out/\n"
            L"/target/\n"
            L"\n"
            L"# --- Language-Specific Dependencies ---\n"
            L"node_modules/\n"
            L"__pycache__/\n"
            L"venv/\n"
            L"\n"
            L"# --- Temp Files & Binaries ---\n"
            L"*.log\n"
            L"*.tmp\n"
            L"*.exe\n"
            L"*.dll\n"
            L"*.so\n"
            L"*.dylib\n"
            L"*.o\n"
            L"*.obj\n"
            L"Thumbs.db\n"
            L".DS_Store\n"
        }}
    };

    auto it = dict.find(key);
    if (it == dict.end()) return L"";
    return (currentLang == Lang::CN) ? it->second.first : it->second.second;
}