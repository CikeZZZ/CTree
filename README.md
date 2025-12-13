CTree - Windows Directory Tree Generator
 
CTree - Windows 目录树生成器
 
License: MIT
 
CTree is a lightweight, high-performance command-line tool for Windows 7 and above, designed to generate clean, readable, and Unicode-safe directory structure trees — with zero runtime dependencies.
CTree 是一款轻量级、高性能的命令行工具，支持 Windows 7 及以上系统，用于生成清晰、易读且兼容全 Unicode 的目录结构树——无需任何运行时依赖。
 
Built specifically for developers and power users:
 
- Instantly export clean project structures (respecting  .gitignore -style rules)
 
- Copy file content or directory trees to clipboard with one click via right-click menu
 
- Perfect for writing documentation, submitting GitHub Issues, or sharing context with AI assistants
 
专为开发者与高效办公用户设计：
 
- 快速导出简洁的项目结构（支持  .gitignore  风格的忽略规则）
 
- 通过右键菜单一键将文件内容或目录树复制到剪贴板
 
- 完美适配编写技术文档、提交 GitHub Issue 或向 AI 助手提供上下文等场景
 
 
 
✨ Features / 功能特性
 
表格  
Feature / 功能 Description / 说明 
High-Performance Tree Generation 高性能目录树生成 Traverses millions of files in seconds with <2MB memory footprint. Outputs to terminal, file, or clipboard with standard  ├── / └──  symbols. 秒级遍历百万级文件，内存占用低于 2MB。支持以标准树形符号（ ├── / └── ）输出到终端、文件或剪贴板。 
Smart Context Menu Integration 智能右键菜单集成 Two zero-friction actions: - Right-click folder or background → “Generate Tree File” - Right-click any text file → “Copy File Content” 无需操作命令行，两步便捷功能： - 右键点击文件夹或文件夹空白处 → “生成目录树文件” - 右键点击任意文本文件 → “复制文件内容” 
Git-Compatible Ignore System Git 风格忽略系统 Supports  .treeignore  with core  .gitignore  syntax: root anchors ( /build ), path matching ( src/temp ), dir-only ( logs/ ), and wildcards ( *.log ). 支持  .treeignore  文件，兼容  .gitignore  核心语法：根目录锚定（ /build ）、路径匹配（ src/temp ）、仅匹配目录（ logs/ ）及通配符（ *.log ）。 
Auto Encoding Detection 自动编码识别 Safely reads and copies text files in UTF-8, UTF-8-BOM, GBK, and more — no garbled Chinese! 可安全读取并复制 UTF-8、UTF-8-BOM、GBK 等编码的文本文件，中文内容无乱码。 
Full Unicode Support 完整 Unicode 支持 End-to-end wide-string pipeline ensures perfect handling of Chinese, Japanese, Korean, Cyrillic, and emoji in paths, filenames, and content. 全链路宽字符处理，完美支持路径、文件名及内容中的中、日、韩、西里尔字母等多语言文字及表情符号。 
Portable & Green 绿色便携，无痕运行 Single  CTree.exe  file (~200KB). No installer, no background process, no DLLs. 仅单个  CTree.exe  文件（约 200KB），无需安装，无后台进程，无外部 DLL 依赖。 
 
 
 
🌳 Output Example / 输出示例
 
CTree uses standard tree indentation symbols ( ├── ,  └── ,  │ ) and appends  \  to folder names for clear visual distinction. It fully supports Unicode paths and filenames:
 
CTree 采用标准树形缩进符号（ ├── 、 └── 、 │ ），并在文件夹名称后添加  \  以实现清晰的视觉区分，且完全支持 Unicode 路径与文件名：
 
text  
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
 
 
💡 This output is format-safe — paste directly into GitHub, Notion, Discord, or email without corruption.
💡 该输出格式稳定，可直接粘贴到 GitHub、Notion、Discord 或邮件中，不会出现格式错乱。
 
 
 
🚀 Quick Start / 快速开始
 
Installation (Portable, No Installer)
 
安装（绿色便携，无需安装）
 
Download  CTree.exe 
👉 GitHub Releases
Place it anywhere (e.g.,  C:\Tools\ ). Add to  PATH  for global CLI access.
 
下载  CTree.exe 
👉 GitHub 发布页
可将其放在任意路径（如  C:\Tools\ ），若需全局调用，可将路径添加到系统  PATH  环境变量中。
 
Enable Right-Click Menu
启用右键菜单：
Run  CTree.exe  → Type  1  → Press Enter.
Two actions will be registered:
 
 
- Right-click on a folder or folder background → “Generate Tree File”
 
- 📌 Smart output location:
 
- Right-click on a folder → tree file created alongside it.
 
- Right-click on background → tree file created inside the current directory.
 
- Right-click on any text file → “Copy File Content”
 
启用右键菜单
运行  CTree.exe  → 输入  1  → 按下回车键。
系统将注册两个右键功能：
 
 
- 右键点击文件夹或文件夹空白处 → “生成目录树文件”
 
- 📌 智能输出路径 ：
 
- 右键点击文件夹本身 → 目录树文件生成在该文件夹同级目录
 
- 右键点击文件夹空白处 → 目录树文件生成在当前目录内
 
- 右键点击任意文本文件 → “复制文件内容”
 
Remove Right-Click Menu
移除右键菜单：
Run  CTree.exe  → Type  2  → Press Enter.
 
💡 Safe & clean: Only modifies current-user registry. No traces left behind.
 
移除右键菜单
运行  CTree.exe  → 输入  2  → 按下回车键。
 
💡 安全无残留：仅修改当前用户的注册表项，卸载后无任何痕迹。
 
 
 
📖 CLI Usage / 命令行用法
 
bash  
CTree.exe [options]
 
 
Common Options / 常用选项
 
表格  
Option / 选项 Description / 说明 
 -i, --input <path>  Directory to scan. Required unless  -c <file>  is used. 指定扫描目录（除非使用  -c  复制单个文件，否则此选项为必填）。 
 -o, --output [path]  Output to file. If omitted, generates  tree_YYYYMMDD_HHMMSS.txt . 指定输出文件路径；若省略路径，将自动生成带时间戳的文件（格式： tree_YYYYMMDD_HHMMSS.txt ）。 
 -c, --copy [path]  • With  -i : copy tree to clipboard. • With file path: copy file content to clipboard. • 配合  -i  使用：将目录树复制到剪贴板 • 若指定文件路径：将该文件内容复制到剪贴板 
 -n, --ignore <pattern>  Add temporary ignore rule (e.g.,  -n "*.log" -n "/temp" ). 添加临时忽略规则（可多次使用，示例： -n "*.log" -n "/temp" ）。 
 -f, --file <path>  Use custom ignore file (e.g.,  -f .gitignore ). 指定自定义忽略配置文件（示例： -f .gitignore ）。 
 -g, --global  Create global  .treeignore  in  %USERPROFILE% . 在用户根目录（ %USERPROFILE% ）创建全局  .treeignore  文件。 
 -l, --local  Create local  .treeignore  in current directory. 在当前目录创建本地  .treeignore  文件。 
 -d, --delete-global  Delete global  .treeignore  if exists. 删除已存在的全局  .treeignore  文件。 
 -h, --help  Show help. 显示帮助信息。 
 -v, --version  Show version. 显示版本信息。 
 
🔁 Ignore Precedence:  -f  (explicit) > local  .treeignore  > global  .treeignore  >  -n  (temp rules)
 
🔁 忽略规则优先级： -f （指定配置文件） > 本地  .treeignore  > 全局  .treeignore  >  -n （临时规则）
 
 
 
🚫 Ignore Rules / 忽略规则说明
 
CTree’s ignore engine supports the most useful subset of  .gitignore  syntax — optimized for clarity and performance.
 
CTree 的忽略引擎支持  .gitignore  语法中最实用的子集，兼顾清晰度与性能。
 
- Comments
Lines with  # ,  // , or  \\  are ignored.
 
gitignore  
# This is a comment
// Ignored line
 
 
- 注释
以  # 、 //  或  \\  开头的行将被视为注释，不会生效。
 
gitignore  
# 这是一条注释
// 这一行会被忽略
 
 
- Path Separators
 /  and  \  are treated identically.
 
- 路径分隔符
 /  与  \  作用完全相同，无需区分系统路径格式。
 
- Root Anchor
 /build  → matches only  build/  in the scan root.
 
- 根目录锚定
 /build  → 仅匹配扫描根目录下的  build/  目录，不匹配子目录中的  build/ 。
 
- Path Match
 src/debug  → matches  src/debug/  anywhere under root.
 
- 路径匹配
 src/debug  → 匹配根目录下所有路径中包含  src/debug/  的目录（无论层级）。
 
- Directory Only
 temp/  → matches directories named  temp , not files.
 
- 仅匹配目录
 temp/  → 仅匹配名为  temp  的目录，不匹配名为  temp  的文件。
 
- Wildcards
 
 
-  *  → single-segment wildcard ( *.log ,  temp* )
 
-  ?  → single-character ( file?.txt )
 
- 通配符
 
 
-  *  → 单段通配符，匹配任意字符（示例： *.log  匹配所有  .log  后缀文件， temp*  匹配所有以  temp  开头的文件/目录）
 
-  ?  → 单字符通配符，匹配单个任意字符（示例： file?.txt  可匹配  file1.txt 、 fileA.txt  等）
 
⚠️ Not supported: Regular expressions, negation ( ! ), escape sequences.
 
⚠️ 暂不支持：正则表达式、否定规则（ ! ）、转义序列。
 
 
 
🧪 Examples / 使用示例
 
Example 1: Export clean project structure using  .gitignore 
 
cmd  
CTree.exe -i "C:\MyProject" -f ".gitignore" -n ".git/" -c
 
 
→ Respects  .gitignore , forces ignore  .git/ , outputs tree to terminal and copies to clipboard.
 
示例 1：使用  .gitignore  导出简洁项目结构
 
cmd  
CTree.exe -i "C:\MyProject" -f ".gitignore" -n ".git/" -c
 
 
→ 遵循  .gitignore  规则，强制忽略  .git/  目录，目录树将输出到终端，同时复制到剪贴板。
 
Example 2: Generate timestamped tree file, ignoring noise
 
cmd  
CTree.exe -i "D:\Code\App" -o -n "node_modules/" "/dist" "*.log"
 
 
→ Creates  tree_20251212_143000.txt  with clean structure.
 
示例 2：生成带时间戳的目录树文件，忽略冗余内容
 
cmd  
CTree.exe -i "D:\Code\App" -o -n "node_modules/" "/dist" "*.log"
 
 
→ 生成格式为  tree_20251212_143000.txt  的文件，目录树已过滤  node_modules/ 、根目录  dist/  及所有  .log  文件。
 
Example 3: Copy file content (auto encoding detection)
 
cmd  
CTree.exe -c "README_zh.md"
 
 
示例 3：复制文件内容（自动编码识别）
 
cmd  
CTree.exe -c "README_zh.md"
 
 
→ 自动识别  README_zh.md  的文件编码，将内容复制到剪贴板，无乱码。
 
Example 4: Initialize ignore template
 
cmd  
CTree.exe -l
 
 
→ Creates  .treeignore  with sensible defaults ( *.exe ,  /bin ,  .git/ , etc.)
 
示例 4：初始化忽略规则模板
 
cmd  
CTree.exe -l
 
 
→ 在当前目录创建  .treeignore  文件，内置常用默认规则（如  *.exe 、 /bin 、 .git/  等）。
 
 
 
❓ FAQ / 常见问题
 
Q: Right-click menu doesn’t appear?
A: Run as standard user (not Admin). Antivirus may block registry writes. Check keys:
 
-  HKEY_CLASSES_ROOT\Directory\Background\shell\CTree 
 
-  HKEY_CLASSES_ROOT\SystemFileAssociations\text\shell\CopyContent 
 
Q：右键菜单未显示怎么办？
A：请以普通用户身份运行（而非管理员），部分杀毒软件可能会拦截注册表写入。可手动检查以下注册表项是否存在：
 
-  HKEY_CLASSES_ROOT\Directory\Background\shell\CTree 
 
-  HKEY_CLASSES_ROOT\SystemFileAssociations\text\shell\CopyContent 
 
Q: Chinese characters look garbled in console?
A: Right-click console title → Properties → Font → Choose Consolas or SimSun.
 
Q：控制台中中文显示乱码如何解决？
A：右键点击控制台标题栏 → 属性 → 字体 → 选择 Consolas（英文）或 宋体（中文）字体。
 
Q: Why recommend x86 version?
A: Yes, use x86!
 
- Smaller pointers → better CPU cache efficiency → faster I/O on large directories
 
- Runs perfectly on all Windows 7+ (32/64-bit) via WOW64
 
- Smaller memory footprint → ideal for I/O-bound tool
 
💡 Unless you need native 64-bit, always choose  CTree-*-win-x86.zip .
 
Q：为何推荐 x86（32位）版本？
A：优先选择 x86 版本！
 
- 更小的指针占用 → 更高的 CPU 缓存效率 → 处理大型目录时 I/O 速度更快
 
- 通过 WOW64 兼容层，可在 所有 Windows 7+（32/64位）系统 上完美运行
 
- 内存占用更低 → 更适合 I/O 密集型工具场景
 
💡 除非需要原生 64 位支持，否则建议始终选择  CTree-*-win-x86.zip 。
 
Q: How is this better than Windows  tree ?
A:  tree  breaks on Unicode, has no ignore rules, can’t copy to clipboard. CTree is built for real-world dev workflows.
 
Q：与 Windows 自带  tree  命令相比，CTree 有何优势？
A：系统自带  tree  命令存在 Unicode 兼容性问题、无忽略规则、无法复制到剪贴板，而 CTree 是为实际开发工作流量身打造的工具，可解决上述所有问题。
 
 
 
📜 License / 许可证
 
MIT License — free for personal and commercial use.
See LICENSE
 
MIT 许可证 — 个人与商业用途均免费。
查看许可证详情
 
 
 
💬 Contributions Welcome!
Want JSON/YAML output? PowerShell alias? HTML with collapsible nodes?
Open an issue or PR — let’s make CTree even more powerful!
💬 欢迎贡献！
希望支持 JSON/YAML 格式输出？PowerShell 别名？带折叠节点的 HTML 格式？
欢迎提交 Issue 或 Pull Request，一起让 CTree 变得更强大！
