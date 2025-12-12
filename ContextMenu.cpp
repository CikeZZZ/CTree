#include "ContextMenu.h"
#include "Lang.h"
#include "Utils.h"

/// @brief 获取当前可执行文件的完整路径
/// @return 可执行文件的绝对路径（宽字符串）
std::wstring GetExePath() {
    wchar_t buf[MAX_PATH];
    GetModuleFileNameW(NULL, buf, MAX_PATH); // NULL 表示当前模块
    return std::wstring(buf);
}

/// @brief 在注册表中注册一个子菜单项（带父菜单分组）
/// @param hRoot 注册表根键（如 HKEY_CURRENT_USER）
/// @param parentPath 父级注册表路径（如 "Software\\Classes\\Directory\\shell"）
/// @param keyName 子菜单的键名（唯一标识，如 "TreeGen.Tree"）
/// @param menuName 显示在右键菜单中的名称（本地化）
/// @param cmd 要执行的命令行（含参数，如 "treegen.exe -i \"%V\""）
/// 
/// @note 会自动创建父菜单项（如 "Tree & Copy"），并将其设为子命令容器（SubCommands）
void RegMenuKey(HKEY hRoot, const std::wstring& parentPath, const std::wstring& keyName, const std::wstring& menuName, const std::wstring& cmd) {
    HKEY hParent, hKey, hCmd;

    // 构造完整父菜单路径：parentPath + \PARENT_MENU_NAME
    // 例如：Directory\shell\Tree & Copy
    std::wstring fullParent = parentPath + L"\\" + PARENT_MENU_NAME;

    // 创建父菜单项
    if (RegCreateKeyExW(hRoot, fullParent.c_str(), 0, NULL, 0, KEY_WRITE, NULL, &hParent, NULL) == ERROR_SUCCESS) {
        // 设置父菜单显示名称（支持 MUI 多语言）
        std::wstring title = PARENT_MENU_NAME;
        RegSetValueExW(hParent, L"MUIVerb", 0, REG_SZ,
            (BYTE*)title.c_str(), (DWORD)(title.size() + 1) * 2);

        // 声明该菜单为“子命令容器”（必须设 SubCommands 为空字符串）
        RegSetValueExW(hParent, L"SubCommands", 0, REG_SZ, (BYTE*)L"", 2);

        // 创建子菜单项（在父菜单的 shell 下）
        std::wstring subKeyPath = L"shell\\" + keyName;
        if (RegCreateKeyExW(hParent, subKeyPath.c_str(), 0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL) == ERROR_SUCCESS) {
            // 设置子菜单显示名称
            RegSetValueExW(hKey, L"MUIVerb", 0, REG_SZ,
                (BYTE*)menuName.c_str(), (DWORD)(menuName.size() + 1) * 2);

            // 创建 command 子键，并写入实际命令
            if (RegCreateKeyExW(hKey, L"command", 0, NULL, 0, KEY_WRITE, NULL, &hCmd, NULL) == ERROR_SUCCESS) {
                RegSetValueExW(hCmd, NULL, 0, REG_SZ,
                    (BYTE*)cmd.c_str(), (DWORD)(cmd.size() + 1) * 2);
                RegCloseKey(hCmd);
            }
            RegCloseKey(hKey);
        }
        RegCloseKey(hParent);
    }
}

/// @brief 安全删除已注册的菜单项（避免留下空壳）
/// @param hRoot 注册表根键
/// @param parentRoot 父级路径（如 "Software\\Classes\\Directory\\shell"）
/// @note 会先删除两个子菜单（Tree 和 Copy），再检查 shell 下是否还有其他子项：
///       - 如果没有，则连父菜单一起删掉，保持注册表干净！
void DeleteMenuKeySafe(HKEY hRoot, const std::wstring& parentRoot) {
    std::wstring fullParent = parentRoot + L"\\" + PARENT_MENU_NAME;
    std::wstring shellPath = fullParent + L"\\shell";

    // 删除两个具体的子菜单项
    RegDeleteTreeW(hRoot, (shellPath + L"\\" + MENU_TREE_KEY).c_str());
    RegDeleteTreeW(hRoot, (shellPath + L"\\" + MENU_COPY_KEY).c_str());

    // 检查 shell 下是否还有残留子项
    HKEY hShell;
    if (RegOpenKeyExW(hRoot, shellPath.c_str(), 0, KEY_READ, &hShell) == ERROR_SUCCESS) {
        DWORD subKeyCount = 0;
        RegQueryInfoKeyW(hShell, NULL, NULL, NULL, &subKeyCount, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        RegCloseKey(hShell);

        // 若无其他子项，则可以安全删除整个父菜单
        if (subKeyCount == 0) {
            RegDeleteTreeW(hRoot, fullParent.c_str());
        }
    }
    else {
        // 如果 shell 本身就不存在，说明可能已被删，但仍尝试清理父菜单
        RegDeleteTreeW(hRoot, fullParent.c_str());
    }
}

/// @brief 安装右键菜单项
/// @note 注册以下位置：
///       1. 文件夹空白处右键（Directory\\Background\\shell）
///       2. 文件夹图标右键（Directory\\shell）
///       3. 文本文件右键（SystemFileAssociations\\text\\shell）用于“复制路径”
void InstallMenus() {
    std::wstring exe = L"\"" + GetExePath() + L"\""; // 用引号包裹路径，防空格

    // === 注册“生成目录树”菜单 ===
    std::wstring cmdTree = exe + L" -i \"%V\" -o";   // %V = 文件夹路径（Background 用）
    std::wstring nameTree = Strings::get("CTX_TREE_NAME"); // 本地化名称，如“生成目录树”

    // 注册到“文件夹空白处”右键
    RegMenuKey(HKEY_CURRENT_USER, L"Software\\Classes\\Directory\\Background\\shell",
        MENU_TREE_KEY, nameTree, cmdTree);
    // 注册到“文件夹图标”右键
    RegMenuKey(HKEY_CURRENT_USER, L"Software\\Classes\\Directory\\shell",
        MENU_TREE_KEY, nameTree, cmdTree);

    // === 注册“复制文件路径”菜单（仅文本文件）===
    std::wstring cmdCopy = exe + L" -c \"%1\"";      // %1 = 选中文件路径
    std::wstring nameCopy = Strings::get("CTX_COPY_NAME"); // 如“复制路径到剪贴板”

    RegMenuKey(HKEY_CURRENT_USER, L"Software\\Classes\\SystemFileAssociations\\text\\shell",
        MENU_COPY_KEY, nameCopy, cmdCopy);
}

/// @brief 卸载所有已注册的右键菜单
void UninstallMenus() {
    // 从三个位置安全删除菜单
    DeleteMenuKeySafe(HKEY_CURRENT_USER, L"Software\\Classes\\Directory\\Background\\shell");
    DeleteMenuKeySafe(HKEY_CURRENT_USER, L"Software\\Classes\\Directory\\shell");
    DeleteMenuKeySafe(HKEY_CURRENT_USER, L"Software\\Classes\\SystemFileAssociations\\text\\shell");
}

/// @brief 显示交互式命令行菜单（用于安装/卸载）
void ShowInteractiveMenu() {
    system("cls"); // 清屏（Windows）
    std::cout << to_utf8(Strings::get("MENU_TITLE")) << "\n-------------------------\n";
    std::cout << to_utf8(Strings::get("MENU_OPT_1")) << '\n'  // 1. 安装右键菜单
        << to_utf8(Strings::get("MENU_OPT_2")) << "\n-------------------------\n";
    std::cout << to_utf8(Strings::get("INPUT_PROMPT")); // 请输入选项：

    char choice;
    while (std::cin >> choice) {
        if (choice == '1') {
            InstallMenus();
            std::cout << to_utf8(Strings::get("SUCCESS_ADD")) << std::endl; // 安装成功！
            system("pause"); // 按任意键继续
            break;
        }
        else if (choice == '2') {
            UninstallMenus();
            std::cout << to_utf8(Strings::get("SUCCESS_REM")) << std::endl; // 卸载成功！
            system("pause");
            break;
        }
        else {
            std::cout << to_utf8(Strings::get("MENU_OPT_ERROR")) << std::endl; // 无效选项
        }
    }
}