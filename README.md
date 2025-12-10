# CTree - Windows Directory Tree Generator  
# CTree - Windows 目录树生成器

[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)

CTree is a lightweight, high-performance command-line tool for Windows 7 and above, designed to generate clean and readable directory structure trees.  
CTree 是一款轻量级、高性能的命令行工具，支持 Windows 7 及以上系统，用于生成清晰美观的文件目录结构树。

It is built specifically for developers: not only can it quickly export project structures, but it also supports copying file contents to the clipboard with one click—ideal for writing technical documentation, submitting issues, or providing code context to AI assistants.  
它专为开发者打造：不仅能快速导出项目结构，还支持将文件内容一键复制到剪贴板，非常适合编写技术文档、提交 Issue 或向 AI 助手提供上下文代码。

---

## ✨ Features / 功能特性

| Feature / 功能 | Description / 说明 |
|----------------|--------------------|
| **Tree Generation**<br>**目录树生成** | Quickly generates indented, symbol-enhanced directory trees. Output can be sent to terminal, file, or clipboard. <br> 快速生成带缩进和符号的目录结构，支持输出到终端、文件或剪贴板。 |
| **Context Menu Integration**<br>**右键菜单集成** | Registers two convenient actions into Windows Explorer’s right-click menu—no command line needed. <br> 将两个便捷操作注册到 Windows 资源管理器右键菜单，无需打开命令行。 |
| **Smart Ignore System**<br>**智能忽略系统** | Supports `.treeignore` configuration files with syntax compatible with core `.gitignore` rules (root anchors, wildcards, path matching). <br> 支持 `.treeignore` 配置文件，语法兼容 `.gitignore` 核心规则（根锚定、通配符、路径匹配）。 |
| **Content Extraction**<br>**文件内容提取** | Automatically detects text encoding (UTF-8, GBK, BOM, etc.) and safely copies content to the clipboard. <br> 自动识别文本编码（UTF-8/GBK/BOM 等），安全地将内容复制到剪贴板。 |
| **Full Chinese Support**<br>**完全中文支持** | Full Unicode/UTF-8 pipeline ensures perfect handling of Chinese paths, filenames, and content. <br> 全流程 Unicode/UTF-8 内核，完美支持中文路径、文件名及内容。 |

---

## 🚀 Quick Start / 快速开始

### Installation & Integration (Portable, No Installer)  
### 安装与集成（绿色便携，无需安装）

1. Download `CTree.exe` and place it in any directory (adding to `PATH` is recommended).  
   下载 `CTree.exe` 并放置在任意目录（建议加入系统 `PATH` 环境变量）。

2. **Add to Right-Click Menu**:  
   **添加右键菜单**：
   ```cmd
   CTree.exe
   ```
   In the interactive menu, enter `1` to register two context menu items:  
   在交互界面中输入 `1`，注册以下两个右键功能：
   - Right-click on **folder background** → “Generate Tree File”  
     在**文件夹空白处右键** → “生成目录树文件”
   - Right-click on **any text file** → “Copy File Content”  
     在**任意文本文件上右键** → “复制文件内容”

3. **Remove from Right-Click Menu**:  
   **移除右键菜单**：  
   Run `CTree.exe` and enter `2` to cleanly remove all registry entries.  
   运行 `CTree.exe` 并输入 `2`，即可彻底清理注册表残留。

> 💡 Note: Context menu integration modifies only the current user’s registry hive and leaves no background processes.  
> 💡 注：右键菜单仅修改当前用户的注册表，无后台进程，安全可靠。

---

## 📖 CLI Usage / 命令行用法

```bash
CTree.exe [command] [options]
```

### Common Options / 常用选项

| Option / 选项 | Description / 说明 |
|---------------|--------------------|
| `-i, --input <path>` | Specify the directory to scan. Required unless using `-c` with a single file. <br> 指定要扫描的目录（除非使用 `-c` 复制单个文件，否则必填）。 |
| `-o, --output [path]` | Output tree to a file. If no path is given, auto-generates `tree_YYYYMMDD_HHMMSS.txt`. <br> 输出到文件；若省略路径，自动生成带时间戳的文件名（如 `tree_20251210_210000.txt`）。 |
| `-c, --copy [path]` | <ul><li>With `-i`: copies the generated tree to clipboard.</li><li>With a file path: copies that file’s content to clipboard.</li></ul> <br> <ul><li>配合 `-i`：将生成的目录树复制到剪贴板。</li><li>指定文件路径：复制该文件内容到剪贴板。</li></ul> |
| `-n, --ignore <pattern>` | Add temporary ignore patterns (can be used multiple times). <br> 临时添加忽略规则（可多次使用，如 `-n "*.log" -n "/temp"`）。 |
| `-g, --global` | Create a global `.treeignore` template in `%USERPROFILE%`. <br> 在用户目录（`%USERPROFILE%`）创建全局 `.treeignore` 模板。 |
| `-l, --local` | Create a local `.treeignore` template in the current working directory. <br> 在当前工作目录创建本地 `.treeignore` 模板。 |

---

## 🚫 Ignore Rules / 忽略规则说明

CTree uses `.treeignore` files to filter out unwanted files or directories. The rule syntax is compatible with core `.gitignore` logic.  
CTree 使用 `.treeignore` 文件过滤不需要显示的文件或目录，规则语法兼容 `.gitignore` 核心逻辑。

- **Comments**: Lines starting with `#`, or containing `//` or `\\`, are ignored.  
  **注释**：以 `#` 开头，或包含 `//`、`\\` 的行将被忽略。
  ```gitignore
  # This is a comment
  // This line is also ignored
  ```

- **Path Separators**: `/` and `\` are treated as equivalent internally.  
  **路径分隔符**：系统内部将 `/` 和 `\` 视为完全等价。

- **Root Anchor**: Rules starting with `/` (e.g., `/build`) match only in the root directory.  
  **根锚定**：以 `/` 开头的规则（如 `/build`）仅匹配根目录下的项。

- **Path Match**: Rules containing slashes in the middle (e.g., `src/debug`) match relative paths.  
  **路径匹配**：中间包含斜杠的规则（如 `src/debug`）匹配相对路径。

- **Directory Match**: Rules ending with `/` (e.g., `temp/`) match only directories.  
  **目录匹配**：以 `/` 结尾的规则（如 `temp/`）仅匹配目录。

- **Wildcards**: Supports `*` (single-level) and limited `**` (multi-level, experimental).  
  **通配符**：支持 `*`（单层匹配）和有限的 `**`（多层匹配，实验性）。
  ```gitignore
  *.tmp         # Ignores all .tmp files
  logs/**/*.log # Ignores .log files under logs/ at any depth (if ** is supported)
  ```

> ⚠️ Note: `.treeignore` precedence: explicit (-f) > local > global. Regular expressions are **not** supported.  
> ⚠️ 注意：`.treeignore` 优先级：指定(-f) > 本地 > 全局。**不支持正则表达式**。

---

## 🧪 Examples / 使用示例

### Example 1: Export project tree while ignoring build artifacts  
### 示例 1：导出项目结构并忽略构建产物
```cmd
CTree.exe -i "D:\MyProject" -o -n "node_modules/" -n "/dist" -n "*.log"
```
→ Generates `tree_20251210_210000.txt`, excluding `node_modules`, root-level `dist`, and log files.  
→ 自动生成 `tree_20251210_210000.txt`，排除 `node_modules`、根目录 `dist` 和日志文件。

### Example 2: Copy README.md content to clipboard  
### 示例 2：将 README.md 内容复制到剪贴板
```cmd
CTree.exe -c "D:\MyProject\README.md"
```

### Example 3: Generate tree and copy directly to clipboard (for pasting into chat/docs)  
### 示例 3：生成树并直接复制到剪贴板（用于粘贴到聊天或文档）
```cmd
CTree.exe -i "C:\Code\App" -c
```

### Example 4: Initialize a local ignore file  
### 示例 4：初始化本地忽略规则
```cmd
CTree.exe -l
```
→ Creates `.treeignore` in the current directory with common defaults (e.g., `*.exe`, `/bin`, `.git/`).  
→ 在当前目录创建 `.treeignore`，包含常用默认规则（如 `*.exe`, `/bin`, `.git/` 等）。

---

## ❓ FAQ / 常见问题

**Q: The right-click menu doesn’t appear. What should I do?**  
**Q：右键菜单没有出现怎么办？**  
A: Run `CTree.exe` as a standard user (not Administrator), and ensure your antivirus isn’t blocking registry writes. You can manually verify these registry keys:  
A：请以普通用户身份运行 `CTree.exe`（不要用管理员），并确保杀毒软件未拦截注册表写入。可手动检查以下注册表项：  
- `HKEY_CLASSES_ROOT\Directory\Background\shell\CTree`  
- `HKEY_CLASSES_ROOT\SystemFileAssociations\text\shell\CopyContent`

**Q: Why do I see garbled Chinese characters?**  
**Q：中文显示乱码？**  
A: You can try right-clicking on the title of the window to check if the default font is a dot matrix font. Please change it to a font that supports Chinese.  
A：可以尝试右键窗口标题处，查看默认值中的字体是不是点阵字体，请换成支持中文的字体。

**Q: Can I recursively ignore specific files in subdirectories?**  
**Q：能否递归忽略子目录中的特定文件？**  
A: Yes. Use `*.test` to match all `test` files at any level. Full `**` support is limited—prefer simple patterns when possible.  
A：可以。使用 `*.test` 可匹配任意层级的 `test` 文件。`**` 支持有限，建议优先使用简单通配符。

**Q: How is this different from the built-in `tree` command?**  
**Q：和 Windows 自带 `tree` 命令有什么区别？**  
A: The native `tree` lacks ignore rules, often breaks with non-ASCII names, and cannot copy content. CTree is purpose-built for modern development workflows.  
A：系统自带 `tree` 不支持忽略规则、中文易乱码、无法复制内容。CTree 专为现代开发场景优化。

**Q: Which version is recommended for use?**  
**Q：推荐使用哪个版本？**  
A: We highly recommend the x86 (32-bit) version.  
A: 强烈推荐使用 x86（32 位）版本。  

* CTree is an I/O-intensive tool focused on directory traversal. The 32-bit build uses smaller pointers and a more compact memory layout, which often leads to better performance when scanning directories with thousands of files—thanks to higher CPU cache efficiency.  
* CTree 是一款以目录遍历为核心的 I/O 密集型工具。32 位版本因指针更小、内存布局更紧凑，在扫描包含大量文件的目录时，往往因更高的 CPU 缓存命中率而表现更优。  

* Additionally, the x86 version runs perfectly on all Windows 7 and later systems (both 32-bit and 64-bit) via the built-in WOW64 compatibility layer—no drawbacks, just wider compatibility and better real-world speed.  
* 此外，x86 版本通过 Windows 内置的 WOW64 兼容层，可在所有 Windows 7 及以上系统（包括 64 位）上完美运行——兼容性更强，实际速度更快，毫无劣势。  

> 💡 Tip: Unless you have a specific requirement for a native 64-bit executable, choose `CTree-*-win-x86.zip`.  
> 💡 提示：除非你明确需要原生 64 位程序，否则请优先选择 `CTree-*-win-x86.zip`。  

---

## 📜 License / 许可证

This project is licensed under the [MIT License](LICENSE)—free for personal and commercial use.  
本项目采用 [MIT 许可证](LICENSE) —— 免费用于个人及商业用途。

---

> 💬 **Contributions & Feedback Welcome!**  
> If you have feature requests, bug reports, or ideas (e.g., JSON/YAML output, PowerShell alias), please open an issue or PR.  
> 💬 **欢迎贡献与反馈！**  
> 如果你有功能建议、发现 Bug，或希望支持更多格式（如 JSON/YAML 输出、PowerShell 别名），请提交 Issue 或 PR。

---
