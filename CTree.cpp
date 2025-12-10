// ============================================================================
// CTree - 目录树生成工具 v1.0.1
// ============================================================================
// 功能：生成文件夹结构树，支持 .gitignore 语法忽略，支持右键菜单，支持剪贴板
// 兼容：Windows 7+ (MSVC 编译)
// ============================================================================

#define WINVER 0x0601
#define _WIN32_WINNT 0x0601  

#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <string>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <map>
#include <sstream>

// Windows Headers
#include <windows.h>
#include <shlwapi.h>
#include <shlobj.h> 

// 链接库 (MSVC)
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Advapi32.lib")
#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "User32.lib")
#pragma comment(lib, "Ole32.lib")

namespace fs = std::filesystem;

// ============================================================================
// [Section 1] 常量、语言包与配置
// ============================================================================

const std::wstring PARENT_MENU_NAME = L"CikeZZZ";
const std::wstring MENU_TREE_KEY = L"CTree";
const std::wstring MENU_COPY_KEY = L"CopyContent";
const std::wstring IGNORE_FILENAME = L".treeignore";
const std::wstring VERSION = L"1.0.1";

enum class Lang { CN, EN };

Lang detect_system_language() {
    LANGID langId = GetUserDefaultUILanguage();
    if ((langId & 0xFF) == LANG_CHINESE) return Lang::CN;
    return Lang::EN;
}

struct Strings {
    static std::wstring get(const std::string& key) {
        static Lang currentLang = detect_system_language();
        static const std::map<std::string, std::pair<std::wstring, std::wstring>> dict = {
            {"MENU_TITLE", {L"[CTree 安装/卸载 - 当前用户]", L"[CTree Install/Uninstall - Current User]"}},
            {"MENU_OPT_1", {L"1. 添加到右键菜单 (文件夹生成树 + 文本文件复制)", L"1. Add to Context Menu (Folder Tree + Text Copy)"}},
            {"MENU_OPT_2", {L"2. 从右键菜单移除", L"2. Remove from Context Menu"}},
            {"MENU_OPT_ERROR",{L"输入有误，请重新输入", L"Input is incorrect. Please re-enter."}},
            {"INPUT_PROMPT", {L"请输入选项: ", L"Enter choice: "}},
            {"SUCCESS_ADD", {L"成功！已添加到右键菜单。", L"Success! Added to context menu."}},
            {"SUCCESS_REM", {L"成功！已清理相关右键菜单。", L"Success! Cleaned up context menu."}},

            {"CTX_TREE_NAME", {L"生成目录树文件", L"Generate Tree File"}},
            {"CTX_COPY_NAME", {L"复制文件内容", L"Copy File Content"}},

            {"ERR_PATH", {L"错误：路径不存在。", L"Error: Path does not exist."}},
            {"ERR_Args", {L"错误：参数不正确。", L"Error: Invalid arguments."}},
            {"ERR_FILE_READ", {L"错误：无法读取文件：", L"Error: Cannot read file: "}},
            {"ERR_FILE_OPEN", {L"错误：无法打开文件：",L"Error: Cannot open file: "}},
            {"MSG_CLIPBOARD", {L"内容已复制到剪贴板。", L"Content copied to clipboard."}},
            {"MSG_SAVED", {L"文件已保存至: ", L"File saved to: "}},
            {"GENERATED_TREEIGNORE",{L"创建完成：", L"Created: "}},
            {"INFO_REM_GLOBAL", {L"已删除全局 .treeignore", L"Removed global .treeignore"}},
            {"PROCESSING", {L"正在处理...", L"Processing..."}},

            {"USING_IGNORE",{L"忽略配置文件：", L"Ignore config file: "}},

            {"HELP_MSG", {
                // 中文版
                L"用法: CTree [命令] [参数]\n"
                L"  -h, --help                 显示此帮助消息\n"
                L"  -v, --version              显示软件版本\n"
                L"  -i, --input <path>         指定输入目录\n"
                L"  -o, --output [path]        1. 输出到文件（可选路径；若省略，则生成带时间戳的文件）\n"
                L"                             2. 若未指定 -o，默认输出到终端\n"
                L"  -c, --copy [path]          1. 配合 -i 使用：不指定 [path] 时，将生成的目录树复制到剪贴板\n"
                L"                             2. 指定 [path] 时：将该文件内容复制到剪贴板\n"
                L"  -n, --ignore [pattern] ... 临时添加忽略规则（可多次使用）\n"
                L"  -f, --file <path>          使用指定的忽略配置文件（兼容大部分 .gitignore 语法）\n"
                L"  -g, --global               创建全局 .treeignore 配置文件\n"
                L"  -l, --local                在当前目录创建本地 .treeignore 配置文件\n"
                L"  -d, --delete-global        删除全局 .treeignore 配置文件\n"
                L"忽略配置文件优先级：指定 (-f) > 本地 > 全局\n",

                // 英文版
                L"Usage: CTree [command] [args]\n"
                L"  -h, --help                 Display this help message\n"
                L"  -v, --version              Show software version\n"
                L"  -i, --input <path>         Specify input directory\n"
                L"  -o, --output [path]        1. Output to file (optional path; if omitted, a timestamped filename is generated)\n"
                L"                             2. If -o is not specified, output to terminal\n"
                L"  -c, --copy [path]          1. With -i: omit [path] to copy the generated tree to clipboard\n"
                L"                             2. If [path] is provided, copy its content to clipboard\n"
                L"  -n, --ignore [pattern] ... Add temporary ignore patterns (can be used multiple times)\n"
                L"  -f, --file <path>          Use an explicit ignore config file (Compatible with most.gitignore syntax)\n"
                L"  -g, --global               Create global .treeignore config file\n"
                L"  -l, --local                Create local .treeignore config file in current directory\n"
                L"  -d, --delete-global        Delete global .treeignore config file\n"
                L"Ignore config priority: explicit (-f) > local > global\n",
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

                // 英文版
                L"# ========================================================\n"
                L"# CTree Ignore Rules Configuration\n"
                L"# ========================================================\n"
                L"# Syntax Guide (Based on .gitignore logic):\n"
                L"#\n"
                L"# [Note] / and \\ are treated as equivalent internally.\n"
                L"#\n"
                L"# 1. Comments & Invalid Lines\n"
                L"#    - Lines starting with # are ignored.\n"
                L"#    - Lines containing consecutive slashes (//, \\\\, /\\, \\/) are ignored.\n"
                L"#\n"
                L"# 2. Root Anchor (Starts with / or \\)\n"
                L"#    Example: /build\n"
                L"#    Effect:  Matches 'build' ONLY at the root level.\n"
                L"#             (Does NOT match sub/build)\n"
                L"#\n"
                L"# 3. Nested Path (Contains / or \\ in the middle)\n"
                L"#    Example: src/temp\n"
                L"#    Effect:  Matches 'src/temp' at ANY depth.\n"
                L"#             (Matches src/temp AND code/src/temp)\n"
                L"#\n"
                L"# 4. Directory Only (Ends with / or \\)\n"
                L"#    Example: log/\n"
                L"#    Effect:  Matches directories named 'log' anywhere,\n"
                L"#             but ignores files named 'log'.\n"
                L"#\n"
                L"# 5. Filename/General (No separators)\n"
                L"#    Example: *.log or debug\n"
                L"#    Effect:  Matches files or directories with this name\n"
                L"#             at any level.\n"
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
                L"# --- Build & Output (Root anchored recommended) ---\n"
                L"/build/\n"
                L"/dist/\n"
                L"/bin/\n"
                L"/obj/\n"
                L"/out/\n"
                L"/target/\n"
                L"\n"
                L"# --- Language Dependencies ---\n"
                L"node_modules/\n"
                L"__pycache__/\n"
                L"venv/\n"
                L"\n"
                L"# --- Temporary & Binaries ---\n"
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
            }}
        };
        auto it = dict.find(key);
        if (it == dict.end()) return L"";
        return (currentLang == Lang::CN) ? it->second.first : it->second.second;
    }
};

// ============================================================================
// [Section 2] 通用工具：编码转换、剪贴板、多路输出
// ============================================================================

std::string to_utf8(const std::wstring& wstr) {
    if (wstr.empty()) return {};
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), nullptr, 0, nullptr, nullptr);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), &strTo[0], size_needed, nullptr, nullptr);
    return strTo;
}

std::wstring to_wide(const std::string& str) {
    if (str.empty()) return {};
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), nullptr, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

// 写入剪贴板 (宽字符优先)
void CopyToClipboardW(const std::wstring& wContent) {
    if (wContent.empty()) return;
    if (!OpenClipboard(nullptr)) return;
    EmptyClipboard();
    size_t size = (wContent.size() + 1) * sizeof(wchar_t);
    HGLOBAL hGlob = GlobalAlloc(GMEM_MOVEABLE, size);
    if (hGlob) {
        void* pLocked = GlobalLock(hGlob);
        if (pLocked) {
            memcpy(pLocked, wContent.c_str(), size);
            GlobalUnlock(hGlob);
            if (!SetClipboardData(CF_UNICODETEXT, hGlob)) GlobalFree(hGlob);
        }
        else {
            GlobalFree(hGlob);
        }
    }
    CloseClipboard();
    std::cout << to_utf8(Strings::get("MSG_CLIPBOARD")) << std::endl;
}

// 写入剪贴板 (UTF-8 转 宽字符)
void CopyToClipboard(const std::string& utf8Content) {
    if (utf8Content.empty()) return;
    CopyToClipboardW(to_wide(utf8Content));
}

// 多路输出控制器 (终端 + 文件 + 剪贴板流)
class MultiWriter {
    std::ostream* _console = nullptr;
    std::ostream* _file = nullptr;
    std::wstringstream* _clipboard = nullptr;
public:
    void setConsole(std::ostream& os) { _console = &os; }
    void setFile(std::ostream& os) { _file = &os; }
    void setClipboard(std::wstringstream& ss) { _clipboard = &ss; }

    void writeLine(const std::wstring& wLine) {
        std::string u8Line = to_utf8(wLine);
        if (_console) *_console << u8Line << '\n';
        if (_file)    *_file << u8Line << '\n';
        if (_clipboard) *_clipboard << wLine << L'\n';
    }
};

// ============================================================================
// [Section 3] 核心逻辑：忽略规则 (Gitignore 风格)
// ============================================================================

class TreeIgnore {
    struct Rule {
        std::wstring pattern;
        bool onlyDir;
        bool isRootOnly;
        bool hasSeparator;
    };
public:
    std::vector<Rule> rules;
    fs::path rootPath;

    void set_root(const fs::path& path) { rootPath = path; }

    void add_rule(std::wstring raw) {
        // 1. 去除前后空白
        const wchar_t* ws = L" \t\n\r";
        size_t start = raw.find_first_not_of(ws);
        if (start == std::wstring::npos) return;
        raw.erase(0, start);
        size_t end = raw.find_last_not_of(ws);
        if (end != std::wstring::npos) raw.erase(end + 1);
        if (raw.empty()) return;

        // 2. 统一分隔符为反斜杠 (先归一化，简化后续判断)
        std::replace(raw.begin(), raw.end(), L'/', L'\\');

        // 3. 检查注释和非法路径
        //    - # 开头是 Git 标准注释
        //    - 包含 \\ 说明原字符串有 // (注释) 或者路径写错了 (a//b)，直接跳过
        if (raw[0] == L'#') return;
        if (raw.find(L"\\\\") != std::wstring::npos) return;

        // 4. 判断是否为根路径规则（以 \ 开头）
        bool isRootOnly = false;
        if (raw[0] == L'\\') {
            isRootOnly = true;
            raw.erase(0, 1);
        }

        // 5. 移除前导 .\ (当前目录)
        while (raw.size() >= 2 && raw[0] == L'.' && raw[1] == L'\\') {
            raw.erase(0, 2);
        }
        if (raw.empty()) return;

        // 6. 解析目录限定符 (以 \ 结尾)
        bool onlyDir = false;
        if (!raw.empty() && raw.back() == L'\\') {
            onlyDir = true;
            raw.pop_back();
            if (raw.empty()) return;
        }

        // 7. 检查是否有中间分隔符 (Git语义: 有分隔符则匹配路径，无则匹配文件名)
        bool hasSeparator = (raw.find(L'\\') != std::wstring::npos);

        rules.push_back({ raw, onlyDir, isRootOnly, hasSeparator });
    }

    void load_file(const fs::path& path) {
        if (!fs::exists(path)) return;
        std::cout << to_utf8(Strings::get("USING_IGNORE") + path.wstring()) << '\n';
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open()) return;

        // BOM Check
        char bom[3] = { 0 }; file.read(bom, 3);
        if (!(bom[0] == '\xEF' && bom[1] == '\xBB' && bom[2] == '\xBF')) file.seekg(0);

        std::string line;
        while (std::getline(file, line)) {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            add_rule(to_wide(line));
        }
    }

    bool should_ignore(const fs::path& fullPath, bool isDirectory) const {
        // 计算相对路径
        std::wstring relPathStr;
        if (!rootPath.empty()) {
            std::error_code ec;
            fs::path rel = fs::relative(fullPath, rootPath, ec);
            if (!ec && rel != L".") relPathStr = rel.wstring();
            else relPathStr = fullPath.filename().wstring();
        }
        else {
            relPathStr = fullPath.wstring();
        }

        std::wstring filenameStr = fullPath.filename().wstring();

        for (const auto& rule : rules) {
            if (rule.onlyDir && !isDirectory) continue;

            if (rule.isRootOnly) {
                // 情况 1: 根锚定 (/foo) -> 完全匹配相对路径
                if (PathMatchSpecW(relPathStr.c_str(), rule.pattern.c_str())) return true;
            }
            else if (rule.hasSeparator) {
                // 情况 2: 含路径符 (src/foo) -> 匹配相对路径 或 深层路径
                if (PathMatchSpecW(relPathStr.c_str(), rule.pattern.c_str())) return true;
                std::wstring deepPattern = L"*\\" + rule.pattern;
                if (PathMatchSpecW(relPathStr.c_str(), deepPattern.c_str())) return true;
            }
            else {
                // 情况 3: 纯文件名 (*.log) -> 匹配任意层级的文件名
                if (PathMatchSpecW(filenameStr.c_str(), rule.pattern.c_str())) return true;
            }
        }
        return false;
    }
};

fs::path get_global_ignore_path() {
    wchar_t path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_PROFILE, NULL, 0, path)))
        return fs::path(path) / IGNORE_FILENAME;
    return {};
}

void create_ignore_template(const fs::path& path) {
    std::ofstream out(path);
    if (out.is_open()) {
        out << "\xEF\xBB\xBF"; // UTF-8 BOM
        out << to_utf8(Strings::get("DEFAULT_TREEIGNORE"));
        out.close();
        std::cout << to_utf8(Strings::get("GENERATED_TREEIGNORE")) << to_utf8(path.wstring()) << std::endl;
    }
}

// ============================================================================
// [Section 4] 核心业务：树结构递归生成
// ============================================================================

void generate_tree_recursive(
    const fs::path& path,
    const std::wstring& prefix,
    MultiWriter& writer,
    const TreeIgnore& ignore
) {
    static const std::wstring U_FOLDER = L"\\";
    static const std::wstring U_BRANCH = L"├── ";
    static const std::wstring U_LAST = L"└── ";
    static const std::wstring U_SPACE = L"    ";
    static const std::wstring U_PIPE = L"│   ";

    struct Entry { fs::path p; std::wstring name; bool isDir; };
    std::vector<Entry> entries;
    entries.reserve(50);

    std::error_code ec;
    for (const auto& e : fs::directory_iterator(path, ec)) {
        if (!ignore.should_ignore(e.path(), e.is_directory())) {
            entries.push_back({ e.path(), e.path().filename().wstring(), e.is_directory() });
        }
    }

    std::sort(entries.begin(), entries.end(), [](const Entry& a, const Entry& b) {
        if (a.isDir != b.isDir) return a.isDir > b.isDir;
        return a.name < b.name;
        });

    for (size_t i = 0; i < entries.size(); ++i) {
        bool isLast = (i == entries.size() - 1);
        std::wstring wLine;
        wLine.reserve(prefix.size() + 10 + entries[i].name.size());
        wLine = prefix;
        wLine += (isLast ? U_LAST : U_BRANCH);
        wLine += entries[i].name;
        if (entries[i].isDir) wLine += U_FOLDER;

        writer.writeLine(wLine);

        if (entries[i].isDir) {
            generate_tree_recursive(
                entries[i].p,
                prefix + (isLast ? U_SPACE : U_PIPE),
                writer,
                ignore
            );
        }
    }
}

// ============================================================================
// [Section 5] 系统集成：Windows 注册表菜单管理
// ============================================================================

std::wstring GetExePath() {
    wchar_t buf[MAX_PATH];
    GetModuleFileNameW(NULL, buf, MAX_PATH);
    return std::wstring(buf);
}

void RegMenuKey(HKEY hRoot, const std::wstring& parentPath, const std::wstring& keyName, const std::wstring& menuName, const std::wstring& cmd) {
    HKEY hParent, hKey, hCmd;
    std::wstring fullParent = parentPath + L"\\" + PARENT_MENU_NAME;

    if (RegCreateKeyExW(hRoot, fullParent.c_str(), 0, NULL, 0, KEY_WRITE, NULL, &hParent, NULL) == ERROR_SUCCESS) {
        std::wstring title = PARENT_MENU_NAME;
        RegSetValueExW(hParent, L"MUIVerb", 0, REG_SZ, (BYTE*)title.c_str(), (DWORD)(title.size() + 1) * 2);
        RegSetValueExW(hParent, L"SubCommands", 0, REG_SZ, (BYTE*)L"", 2);

        std::wstring subKeyPath = L"shell\\" + keyName;
        if (RegCreateKeyExW(hParent, subKeyPath.c_str(), 0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL) == ERROR_SUCCESS) {
            RegSetValueExW(hKey, L"MUIVerb", 0, REG_SZ, (BYTE*)menuName.c_str(), (DWORD)(menuName.size() + 1) * 2);
            if (RegCreateKeyExW(hKey, L"command", 0, NULL, 0, KEY_WRITE, NULL, &hCmd, NULL) == ERROR_SUCCESS) {
                RegSetValueExW(hCmd, NULL, 0, REG_SZ, (BYTE*)cmd.c_str(), (DWORD)(cmd.size() + 1) * 2);
                RegCloseKey(hCmd);
            }
            RegCloseKey(hKey);
        }
        RegCloseKey(hParent);
    }
}

void DeleteMenuKeySafe(HKEY hRoot, const std::wstring& parentRoot) {
    std::wstring fullParent = parentRoot + L"\\" + PARENT_MENU_NAME;
    std::wstring shellPath = fullParent + L"\\shell";
    RegDeleteTreeW(hRoot, (shellPath + L"\\" + MENU_TREE_KEY).c_str());
    RegDeleteTreeW(hRoot, (shellPath + L"\\" + MENU_COPY_KEY).c_str());

    HKEY hShell; // 检查是否为空，若空则删除父菜单
    if (RegOpenKeyExW(hRoot, shellPath.c_str(), 0, KEY_READ, &hShell) == ERROR_SUCCESS) {
        DWORD subKeyCount = 0;
        RegQueryInfoKeyW(hShell, NULL, NULL, NULL, &subKeyCount, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        RegCloseKey(hShell);
        if (subKeyCount == 0) RegDeleteTreeW(hRoot, fullParent.c_str());
    }
    else {
        RegDeleteTreeW(hRoot, fullParent.c_str());
    }
}

void InstallMenus() {
    std::wstring exe = L"\"" + GetExePath() + L"\"";
    std::wstring cmdTree = exe + L" -i \"%V\" -o";
    std::wstring nameTree = Strings::get("CTX_TREE_NAME");
    RegMenuKey(HKEY_CURRENT_USER, L"Software\\Classes\\Directory\\Background\\shell", MENU_TREE_KEY, nameTree, cmdTree);
    RegMenuKey(HKEY_CURRENT_USER, L"Software\\Classes\\Directory\\shell", MENU_TREE_KEY, nameTree, cmdTree);

    std::wstring cmdCopy = exe + L" -c \"%1\"";
    std::wstring nameCopy = Strings::get("CTX_COPY_NAME");
    RegMenuKey(HKEY_CURRENT_USER, L"Software\\Classes\\SystemFileAssociations\\text\\shell", MENU_COPY_KEY, nameCopy, cmdCopy);
}

void UninstallMenus() {
    DeleteMenuKeySafe(HKEY_CURRENT_USER, L"Software\\Classes\\Directory\\Background\\shell");
    DeleteMenuKeySafe(HKEY_CURRENT_USER, L"Software\\Classes\\Directory\\shell");
    DeleteMenuKeySafe(HKEY_CURRENT_USER, L"Software\\Classes\\SystemFileAssociations\\text\\shell");
}

void ShowInteractiveMenu() {
    system("cls");
    std::cout << to_utf8(Strings::get("MENU_TITLE")) << "\n-------------------------\n";
    std::cout << to_utf8(Strings::get("MENU_OPT_1")) << '\n' << to_utf8(Strings::get("MENU_OPT_2")) << "\n-------------------------\n";
    std::cout << to_utf8(Strings::get("INPUT_PROMPT"));
    char choice;
    while (std::cin >> choice) {
        if (choice == '1') { InstallMenus(); std::cout << to_utf8(Strings::get("SUCCESS_ADD")) << std::endl; system("pause"); break; }
        else if (choice == '2') { UninstallMenus(); std::cout << to_utf8(Strings::get("SUCCESS_REM")) << std::endl; system("pause"); break; }
        else std::cout << to_utf8(Strings::get("MENU_OPT_ERROR")) << std::endl;
    }
}

// ============================================================================
// [Section 6] 流程控制：配置解析与业务分发
// ============================================================================

struct AppConfig {
    bool showMenu = false;
    bool showHelp = false;
    bool showVersion = false;
    bool isValid = true;
    bool createGlobal = false;
    bool deleteGlobal = false;
    bool createLocal = false;

    fs::path inputPath;
    bool OutputFlag = false;
    fs::path outputPath;

    bool CopyFlag = false;
    fs::path copyFilePath;

    fs::path specifiedIgnoreFile;
    std::vector<std::wstring> tempIgnores;

    void parse(int argc, wchar_t* argv[]) {
        if (argc < 2) { showMenu = true; return; }
        for (int i = 1; i < argc; ++i) {
            std::wstring arg = argv[i];
            if (arg == L"-h" || arg == L"--help") { showHelp = true; return; }
            if (arg == L"-v" || arg == L"--version") { showVersion = true; return; }

            if (arg == L"-i" || arg == L"--input") {
                if (i + 1 < argc) inputPath = argv[++i]; else isValid = false;
            }
            else if (arg == L"-o" || arg == L"--output") {
                OutputFlag = true;
                if (i + 1 < argc && argv[i + 1][0] != L'-') outputPath = argv[++i];
            }
            else if (arg == L"-c" || arg == L"--copy") {
                CopyFlag = true;
                if (i + 1 < argc && argv[i + 1][0] != L'-') copyFilePath = argv[++i];
            }
            else if (arg == L"-n" || arg == L"--ignore") {
                while (i + 1 < argc) {
                    std::wstring next = argv[i + 1];
                    if (!next.empty() && next[0] == L'-') break;
                    tempIgnores.push_back(next);
                    i++;
                }
            }
            else if (arg == L"-f" || arg == L"--file") {
                if (i + 1 < argc) specifiedIgnoreFile = argv[++i];
            }
            else if (arg == L"-g" || arg == L"--global") createGlobal = true;
            else if (arg == L"-d" || arg == L"--delete-global") deleteGlobal = true;
            else if (arg == L"-l" || arg == L"--local") createLocal = true;
        }

        std::error_code ec;
        if (!inputPath.empty()) { fs::path abs = fs::absolute(inputPath, ec); if (!ec) inputPath = abs; }
        if (OutputFlag && !outputPath.empty()) { fs::path abs = fs::absolute(outputPath, ec); if (!ec) outputPath = abs; }
    }
};

void RunTreeGeneration(const AppConfig& cfg) {
    if (!fs::exists(cfg.inputPath)) { std::cout << to_utf8(Strings::get("ERR_PATH")) << std::endl; return; }

    // 1. 加载忽略规则
    TreeIgnore ignoreMgr;
    bool loaded = false;
    if (!cfg.specifiedIgnoreFile.empty()) { ignoreMgr.load_file(cfg.specifiedIgnoreFile); loaded = true; }
    if (!loaded) {
        if (fs::exists(cfg.inputPath / IGNORE_FILENAME)) ignoreMgr.load_file(cfg.inputPath / IGNORE_FILENAME);
        else if (fs::exists(fs::current_path() / IGNORE_FILENAME)) ignoreMgr.load_file(fs::current_path() / IGNORE_FILENAME);
        else ignoreMgr.load_file(get_global_ignore_path());
    }
    for (const auto& r : cfg.tempIgnores) ignoreMgr.add_rule(r);
    ignoreMgr.set_root(cfg.inputPath);

    // 2. 准备输出
    fs::path finalOutPath = cfg.outputPath;
    if (cfg.OutputFlag && finalOutPath.empty()) {
        auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::tm tm; localtime_s(&tm, &now);
        std::wstringstream wss; wss << L"tree_" << std::put_time(&tm, L"%Y%m%d_%H%M%S") << L".txt";
        finalOutPath = fs::current_path() / wss.str();
    }
    if (!finalOutPath.empty()) ignoreMgr.add_rule(finalOutPath.filename().wstring());

    std::cout << to_utf8(Strings::get("PROCESSING")) << std::endl;

    MultiWriter writer;
    std::ofstream outFile;
    if (!cfg.OutputFlag) writer.setConsole(std::cout);
    if (cfg.OutputFlag) {
        outFile.open(finalOutPath, std::ios::binary);
        if (outFile.is_open()) { outFile << "\xEF\xBB\xBF"; writer.setFile(outFile); }
        else std::cerr << to_utf8(Strings::get("ERR_FILE_OPEN") + finalOutPath.wstring()) << std::endl;
    }
    std::wstringstream wClipBuffer;
    if (cfg.CopyFlag) writer.setClipboard(wClipBuffer);

    // 3. 执行
    std::wstring rootName = cfg.inputPath.filename().wstring();
    if (rootName.empty()) rootName = cfg.inputPath.wstring();
    writer.writeLine(rootName + L"\\");

    generate_tree_recursive(cfg.inputPath, L"", writer, ignoreMgr);

    if (cfg.OutputFlag && outFile.is_open()) {
        outFile.close();
        std::cout << to_utf8(Strings::get("MSG_SAVED")) << to_utf8(finalOutPath.wstring()) << std::endl;
    }
    if (cfg.CopyFlag) CopyToClipboardW(wClipBuffer.str());
}

void RunFileContentCopy(const fs::path& filePath) {
    if (!fs::exists(filePath)) { std::cout << to_utf8(Strings::get("ERR_PATH")) << std::endl; return; }
    std::ifstream in(filePath, std::ios::binary);
    if (!in.is_open()) { std::cout << to_utf8(Strings::get("ERR_FILE_READ") + filePath.wstring()) << std::endl; return; }

    char bom[3] = { 0 }; in.read(bom, 3);
    if (!(bom[0] == '\xEF' && bom[1] == '\xBB' && bom[2] == '\xBF')) in.seekg(0);
    std::ostringstream ss; ss << in.rdbuf();
    CopyToClipboard(ss.str());
}

// ============================================================================
// [Section 7] 入口点
// ============================================================================

int wmain(int argc, wchar_t* argv[]) {
    SetConsoleOutputCP(65001); // UTF-8 Console
    AppConfig config;
    config.parse(argc, argv);

    if (config.showHelp) { std::cout << to_utf8(Strings::get("HELP_MSG")); return 0; }
    if (config.showVersion) { std::cout << to_utf8(VERSION); return 0; }
    if (!config.isValid) { std::cout << to_utf8(Strings::get("ERR_Args")) << std::endl; return 1; }
    if (config.showMenu) { ShowInteractiveMenu(); return 0; }

    if (config.createGlobal) create_ignore_template(get_global_ignore_path());
    if (config.deleteGlobal) { fs::path p = get_global_ignore_path(); if (fs::exists(p)) fs::remove(p); else std::cout << to_utf8(Strings::get("INFO_REM_GLOBAL")) << std::endl; }
    if (config.createLocal) create_ignore_template((config.inputPath.empty() ? fs::current_path() : config.inputPath) / IGNORE_FILENAME);

    if (config.CopyFlag && !config.copyFilePath.empty() && config.inputPath.empty()) {
        RunFileContentCopy(config.copyFilePath);
    }
    else if (!config.inputPath.empty()) {
        RunTreeGeneration(config);
    }
    return 0;
}
