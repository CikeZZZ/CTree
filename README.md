# CTree - Directory Tree Generator for Windows
 
CTree 是一个轻量级的 Windows 命令行工具，用于生成目录树结构并支持文件内容复制。  
CTree is a lightweight Windows command-line tool designed for generating directory tree structures and copying file content.  

支持Windows 7及以上。  
Supports Windows 7 and above.   

它专为开发者设计，支持类似 `.gitignore` 的忽略规则，并集成了右键菜单功能。  
It is designed for developers, supporting `.gitignore`-like ignore rules and integrating seamlessly with the context menu.  

## ✨ 功能特性 (Features)

1.  **生成目录树**：快速生成可视化的文件夹结构树，支持输出到终端、文件或剪贴板。  
    **Generate Directory Tree**: Quickly generate visualized folder structures, supporting output to terminal, file, or clipboard.

2.  **智能忽略**：内置强大的忽略逻辑（兼容 `.gitignore` 语法），支持全局配置、本地配置和临时参数。  
    **Smart Ignore**: Built-in powerful ignore logic (compatible with `.gitignore` syntax), supporting global config, local config, and temporary arguments.

3.  **右键菜单集成**：一键将功能添加到 Windows 资源管理器右键菜单（生成树 / 复制文件内容）。  
    **Context Menu Integration**: One-click add to Windows Explorer context menu (Generate Tree / Copy File Content).

4.  **剪贴板支持**：自动处理文本编码（UTF-8/BOM），将生成的树或文件内容直接复制到剪贴板。  
    **Clipboard Support**: Automatically handles text encoding (UTF-8/BOM) to copy the generated tree or file content directly to the clipboard.

## 🚀 使用方法 (Usage)

* 建议加入环境变量使用  
* It is recommended to add environment variables for use.

### 命令行参数 (Command Line Arguments)

```bash
CTree -i "C:\MyProject" -o
```

*   `-i, --input <path>`: 指定要处理的输入目录。  
    Specify the input directory to process.

*   `-o, --output [path]`: 输出结果到文件（如果省略路径，则自动生成带时间戳的文件名）。  
    Output results to a file (if path is omitted, a timestamped filename is automatically generated).

*   `-c, --copy [path]`: 配合 `-i` 使用时复制树结构；单独使用时复制指定文件的内容。  
    Copy tree structure when used with `-i`; copy content of the specified file when used alone.

*   `-n, --ignore [pattern]`: 临时添加忽略规则（例如：`-n "*.log" "dist/"`）。  
    Add temporary ignore patterns (e.g., `-n "*.log" "dist/"`).

*   `-g / -l / -d`: 管理全局或本地的 `.treeignore` 配置文件。  
    Manage global or local `.treeignore` configuration files.

### 忽略规则说明 (Ignore Rules)

CTree 使用 `.treeignore` 文件来过滤不需要的文件或文件夹。  
CTree uses `.treeignore` files to filter out unwanted files or folders.    

*   **路径分隔符**：在本软件中 `/` 与 `\` 完全等价，内部统一处理。  
    **Path Separators**: `/` and `\` are treated as equivalent in this software and are normalized internally.  

*   **注释**：以 `#` 开头的行，或包含连续斜杠（如 `//`, `\\`）的行将被视为注释或无效行。  
    **Comments**: Lines starting with `#`, or lines containing consecutive slashes (e.g., `//`, `\\`) are treated as comments or invalid lines.  

*   **匹配逻辑**：  
    **Matching Logic**:  
    *   `/build`：仅匹配根目录下的 `build`。 (Matches `build` only in the root directory.)  
    *   `src/temp`：匹配路径中包含该结构的相对路径。 (Matches relative paths containing this structure.)  
    *   `node_modules/`：仅匹配目录。 (Matches directories only.)  
    *   `*.log`：匹配任意深度的同名文件。 (Matches files with this name at any depth.)  

## 📦 安装与菜单注册 (Installation & Menu Registration)

1.  直接运行 `CTree.exe`（不带参数）即可进入交互式菜单模式。  
    Run `CTree.exe` directly (without arguments) to enter interactive menu mode.  

2.  输入 `1` 将 CTree 添加到右键菜单。  
    Enter `1` to add CTree to the context menu.  

3.  现在，您可以右键点击任何文件夹选择“生成目录树文件”，或右键点击文件选择“复制文件内容”。  
    Now you can right-click any folder to select "Generate Tree File", or right-click a file to select "Copy File Content".  

## 🛠️ 构建 (Build)

需要支持 C++17 的编译器（建议 MSVC 2019+）以及 Windows SDK。  
Requires a C++17 compliant compiler (MSVC 2019+ recommended) and Windows SDK.  

```cpp
// 依赖库 (Dependencies)
Shlwapi.lib, Advapi32.lib, Shell32.lib, User32.lib, Ole32.lib
```

---
