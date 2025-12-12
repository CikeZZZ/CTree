# CTree - Windows Directory Tree Generator  
# CTree - Windows 目录树生成器

[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)

CTree is a lightweight, high-performance command-line tool for Windows 7 and above, designed to generate clean, readable, and Unicode-safe directory structure trees — **with zero runtime dependencies**.  
CTree 是一款轻量级、高性能的命令行工具，支持 Windows 7 及以上系统，用于生成清晰、美观、全 Unicode 兼容的目录结构树——**无需任何运行时依赖**。

Built specifically for developers and power users:  
- Instantly export clean project structures (respecting `.gitignore`-style rules)  
- Copy file content or directory trees to clipboard with **one click via right-click menu**  
- Perfect for writing documentation, submitting GitHub Issues, or sharing context with AI assistants  

专为开发者与高效用户打造：  
- 一键导出干净的项目结构（支持 `.gitignore` 风格忽略规则）  
- 通过**右键菜单**，一键将文件内容或目录树复制到剪贴板  
- 完美适用于编写技术文档、提交 Issue 或向 AI 助手提供上下文  

---

## ✨ Features / 功能特性

| Feature / 功能 | Description / 说明 |
|----------------|--------------------|
| **High-Performance Tree Generation**<br>**高性能目录树生成** | Traverses millions of files in seconds with **<2MB memory footprint**. Outputs to terminal, file, or clipboard with standard `├──`/`└──` symbols.<br>秒级遍历百万级文件，**内存占用 <2MB**。支持标准树形符号输出到终端、文件或剪贴板。 |
| **Smart Context Menu Integration**<br>**智能右键菜单集成** | Two zero-friction actions:<br>- Right-click **folder or background** → “Generate Tree File”<br>- Right-click **any text file** → “Copy File Content”<br>无需命令行，操作高效直观。 |
| **Git-Compatible Ignore System**<br>**Git 风格忽略系统** | Supports `.treeignore` with core `.gitignore` syntax: root anchors (`/build`), path matching (`src/temp`), dir-only (`logs/`), and wildcards (`*.log`).<br>支持 `.treeignore`，兼容 `.gitignore` 核心语法：根锚定、路径匹配、目录限定、通配符。 |
| **Auto Encoding Detection**<br>**自动编码识别** | Safely reads and copies text files in UTF-8, UTF-8-BOM, GBK, and more — no garbled Chinese!<br>智能识别 UTF-8/GBK/BOM 等编码，中文内容复制不乱码。 |
| **Full Unicode Support**<br>**完整 Unicode 支持** | End-to-end wide-string pipeline ensures perfect handling of Chinese, Japanese, Korean, Cyrillic, and emoji in paths, filenames, and content.<br>全链路宽字符处理，完美支持多语言路径、文件名及内容（含表情符号）。 |
| **Portable & Green**<br>**绿色便携，无痕运行** | Single `CTree.exe` file (~200KB). No installer, no background process, no DLLs.<br>单文件 EXE（约 200KB），无需安装，无后台进程，无外部依赖。 |

---

## 🌳 Output Example / 输出示例

CTree uses **standard tree indentation symbols** (`├──`, `└──`, `│`) and **appends `\` to folder names** for clear visual distinction. It **fully supports Unicode paths and filenames**:

```text
CTreeTest\
├── New Folder\
│   ├── 新建文件夹\
│   │   ├── 1.png
│   │   ├── 2.jpg
│   │   └── test.cpp
│   ├── 1.png
│   ├── 2.jpg
│   └── かんが.cpp
├── New Folder\
│   └── Test\
│       ├── 새폴더만들기\
│       │   ├── New Folder\
│       │   │   ├── 1.png
│       │   │   ├── 5.txt
│       │   │   └── test.cpp
│       │   ├── 4.jpeg
│       │   └── test.cpp
│       ├── 1.png
│       ├── 2.jpg
│       ├── изображение.svg
│       └── test.cpp
├── 1.png
├── 2.jpg
└── 测试.cpp
```

> 💡 This output is **format-safe** — paste directly into GitHub, Notion, Discord, or email without corruption.  
> 💡 该输出**格式稳定**，可直接粘贴到 GitHub、Notion、Discord 或邮件中，不会错乱。

---

## 🚀 Quick Start / 快速开始

### Installation (Portable, No Installer)  
### 安装（绿色便携，无需安装）

1. **Download `CTree.exe`**  
   👉 [GitHub Releases](https://github.com/CikeZZZ/CTree/releases)  
   Place it anywhere (e.g., `C:\Tools\`). **Add to `PATH` for global CLI access**.

2. **Enable Right-Click Menu**  
   **启用右键菜单**：  
   Run `CTree.exe` → Type `1` → Press Enter.  
   Two actions will be registered:
   - **Right-click on a folder or folder background** → “Generate Tree File”  
     - 📌 *Smart output location*:  
       - Right-click **on a folder** → tree file created **alongside** it.  
       - Right-click **on background** → tree file created **inside** the current directory.  
   - **Right-click on any text file** → “Copy File Content”

3. **Remove Right-Click Menu**  
   **移除右键菜单**：  
   Run `CTree.exe` → Type `2` → Press Enter.  
   > 💡 **Safe & clean**: Only modifies current-user registry. No traces left behind.

---

## 📖 CLI Usage / 命令行用法

```bash
CTree.exe [options]
```

### Common Options / 常用选项

| Option / 选项 | Description / 说明 |
|---------------|--------------------|
| `-i, --input <path>` | Directory to scan. **Required** unless `-c <file>` is used.<br>指定扫描目录（除非使用 `-c` 复制单个文件，否则必填）。 |
| `-o, --output [path]` | Output to file. If omitted, generates `tree_YYYYMMDD_HHMMSS.txt`.<br>输出到文件；省略路径则自动生成带时间戳的文件。 |
| `-c, --copy [path]` | • With `-i`: copy tree to clipboard.<br>• With file path: copy file content to clipboard.<br>• 配合 `-i`：复制目录树到剪贴板。<br>• 指定路径：复制文件内容到剪贴板。 |
| `-n, --ignore <pattern>` | Add temporary ignore rule (e.g., `-n "*.log" -n "/temp"`).<br>临时添加忽略规则（可多次使用）。 |
| `-f, --file <path>` | Use custom ignore file (e.g., `-f .gitignore`).<br>使用自定义忽略配置文件。 |
| `-g, --global` | Create global `.treeignore` in `%USERPROFILE%`.<br>在用户目录创建全局忽略模板。 |
| `-l, --local` | Create local `.treeignore` in current directory.<br>在当前目录创建本地忽略模板。 |
| `-d, --delete-global` | Delete global `.treeignore` if exists.<br>删除全局忽略文件。 |
| `-h, --help` | Show help.<br>显示帮助。 |
| `-v, --version` | Show version.<br>显示版本。 |

> 🔁 **Ignore Precedence**: `-f` (explicit) > local `.treeignore` > global `.treeignore` > `-n` (temp rules)

---

## 🚫 Ignore Rules / 忽略规则说明

CTree’s ignore engine supports the most useful subset of `.gitignore` syntax — **optimized for clarity and performance**.

- **Comments**  
  Lines with `#`, `//`, or `\\` are ignored.  
  ```gitignore
  # This is a comment
  // Ignored line
  ```

- **Path Separators**  
  `/` and `\` are treated identically.

- **Root Anchor**  
  `/build` → matches **only** `build/` in the scan root.

- **Path Match**  
  `src/debug` → matches `src/debug/` anywhere under root.

- **Directory Only**  
  `temp/` → matches directories named `temp`, **not files**.

- **Wildcards**  
  - `*` → single-segment wildcard (`*.log`, `temp*`)  
  - `?` → single-character (`file?.txt`)  
  - `**` → multi-level (experimental, use sparingly)

> ⚠️ **Not supported**: Regular expressions, negation (`!`), escape sequences.

---

## 🧪 Examples / 使用示例

### Example 1: Export clean project structure using `.gitignore`
```cmd
CTree.exe -i "C:\MyProject" -f ".gitignore" -n ".git/" -c
```
→ Respects `.gitignore`, forces ignore `.git/`, outputs tree to terminal **and copies to clipboard**.

### Example 2: Generate timestamped tree file, ignoring noise
```cmd
CTree.exe -i "D:\Code\App" -o -n "node_modules/" "/dist" "*.log"
```
→ Creates `tree_20251212_143000.txt` with clean structure.

### Example 3: Copy file content (auto encoding detection)
```cmd
CTree.exe -c "README_zh.md"
```

### Example 4: Initialize ignore template
```cmd
CTree.exe -l
```
→ Creates `.treeignore` with sensible defaults (`*.exe`, `/bin`, `.git/`, etc.)

---

## ❓ FAQ / 常见问题

**Q: Right-click menu doesn’t appear?**  
A: Run as **standard user** (not Admin). Antivirus may block registry writes. Check keys:  
- `HKEY_CLASSES_ROOT\Directory\Background\shell\CTree`  
- `HKEY_CLASSES_ROOT\SystemFileAssociations\text\shell\CopyContent`

**Q: Chinese characters look garbled in console?**  
A: Right-click console title → Properties → Font → Choose **Consolas** or **SimSun**.

**Q: Why recommend x86 version?**  
A: **Yes, use x86!**  
- Smaller pointers → better CPU cache efficiency → **faster I/O on large directories**  
- Runs perfectly on **all Windows 7+ (32/64-bit)** via WOW64  
- Smaller memory footprint → ideal for I/O-bound tool  
> 💡 Unless you need native 64-bit, **always choose `CTree-*-win-x86.zip`**.

**Q: How is this better than Windows `tree`?**  
A: `tree` breaks on Unicode, has no ignore rules, can’t copy to clipboard. CTree is built for **real-world dev workflows**.

---

## 📜 License / 许可证

MIT License — free for personal and commercial use.  
[See LICENSE](LICENSE)

---

> 💬 **Contributions Welcome!**  
> Want JSON/YAML output? PowerShell alias? HTML with collapsible nodes?  
> Open an issue or PR — let’s make CTree even more powerful!  
> 💬 **欢迎贡献！**  
> 希望支持 JSON/YAML？PowerShell 别名？可折叠 HTML？  
> 欢迎提交 Issue 或 PR，一起让 CTree 更强大！
```

---
