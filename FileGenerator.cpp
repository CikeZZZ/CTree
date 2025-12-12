#include "FileGenerator.h"
#include "Lang.h"
#include <iomanip>
#include <sstream> // 必须包含，否则 FormatSize 会报错

// ============================================================================
// [Part 1] 内部数据结构与构建逻辑
// ============================================================================

/// @brief 树节点结构体，用于内存中表示目录/文件树
struct TreeNode {
    std::wstring name;          ///< 文件/目录名（不含路径）
    fs::path relativePath;      ///< 相对于根目录的路径（用于 HTML 中的链接）
    fs::path fullPath;          ///< 完整物理路径（用于计算大小）
    bool isDir;                 ///< 是否为目录
    uintmax_t size;             ///< 大小（字节），目录包含所有子项总和
    std::vector<TreeNode> children; ///< 子节点列表
};

/// @brief 将字节数格式化为人类可读的字符串（如 1.50 MB）
/// @param bytes 字节数
/// @return 格式化后的宽字符串
std::wstring FormatSize(uintmax_t bytes) {
    const double KB = 1024.0;
    const double MB = KB * 1024.0;
    const double GB = MB * 1024.0;

    std::wstringstream ss;
    ss << std::fixed << std::setprecision(2); // 保留两位小数

    if (bytes >= GB)
        ss << (bytes / GB) << L" GB";
    else if (bytes >= MB)
        ss << (bytes / MB) << L" MB";
    else if (bytes >= KB)
        ss << (bytes / KB) << L" KB";
    else
        ss << bytes << L" B";

    return ss.str();
}

/// @brief 递归构建内存中的目录树
/// @param currentPath 当前遍历的路径
/// @param rootPath 根目录（用于计算 relativePath）
/// @param ignore 忽略规则管理器
/// @param node 输出节点（会被填充）
/// @return 本节点的总大小（含子项）
uintmax_t BuildTreeRecursive(const fs::path& currentPath, const fs::path& rootPath, const TreeIgnore& ignore, TreeNode& node) {
    node.fullPath = currentPath;

    // 1. 获取名称：处理根目录（如 C:\）可能 filename() 为空的情况
    node.name = currentPath.filename().wstring();
    if (node.name.empty()) {
        node.name = currentPath.wstring();
        // 移除末尾斜杠，保证显示美观
        if (!node.name.empty() && (node.name.back() == L'\\' || node.name.back() == L'/')) {
            node.name.pop_back();
        }
    }

    // 2. 计算相对路径（用于 HTML 链接）
    std::error_code ec;
    node.relativePath = fs::relative(currentPath, rootPath, ec);
    if (ec)
        node.relativePath = node.name; // 若失败，降级为仅文件名

    node.isDir = fs::is_directory(currentPath, ec);
    node.size = 0;

    // 如果是文件，直接获取大小
    if (!node.isDir) {
        node.size = fs::file_size(currentPath, ec);
        if (ec) node.size = 0;
        return node.size;
    }

    // 遍历目录下的所有子项
    for (const auto& entry : fs::directory_iterator(currentPath, ec)) {
        // 根据忽略规则跳过
        if (ignore.should_ignore(entry.path(), entry.is_directory()))
            continue;

        TreeNode child;
        uintmax_t childSize = BuildTreeRecursive(entry.path(), rootPath, ignore, child);
        node.children.push_back(std::move(child));
        node.size += childSize; // 累加子项大小
    }

    // 排序：目录在前，文件在后；同类按名称升序
    std::sort(node.children.begin(), node.children.end(), [](const TreeNode& a, const TreeNode& b) {
        if (a.isDir != b.isDir)
            return a.isDir > b.isDir; // 目录优先
        return a.name < b.name;       // 名称排序
        });

    return node.size;
}

// ============================================================================
// [Part 2] 文本格式生成器 (Render Text)
// ============================================================================

/// @brief 递归渲染文本格式的树状结构（如 CTree 默认输出）
/// @param node 当前节点
/// @param prefix 当前行的前缀（如 "├── "）
/// @param writer 多路输出控制器
/// @param isRoot 是否为根节点（首行特殊处理）
void RenderTextRecursive(const TreeNode& node, const std::wstring& prefix, MultiWriter& writer, bool isRoot) {
    // 树状图绘制使用的符号（Unicode 风格）
    static const std::wstring U_FOLDER = L"\\";      // 目录标识
    static const std::wstring U_BRANCH = L"├── ";   // 分支节点
    static const std::wstring U_LAST = L"└── ";     // 最后一个子节点
    static const std::wstring U_SPACE = L"    ";    // 空白填充
    static const std::wstring U_PIPE = L"│   ";     // 纵向连接线

    if (isRoot) {
        // 根节点：直接输出名字 + 目录标识
        writer.writeLine(node.name + U_FOLDER);
        // 递归渲染子节点
        for (size_t i = 0; i < node.children.size(); ++i) {
            bool isLast = (i == node.children.size() - 1);
            RenderTextRecursive(node.children[i], isLast ? U_LAST : U_BRANCH, writer, false);
        }
        return;
    }

    // 非根节点：输出前缀 + 名字
    std::wstring line = prefix + node.name + (node.isDir ? U_FOLDER : L"");
    writer.writeLine(line);

    // 如果是目录且有子项，继续递归
    if (node.isDir) {
        // 计算下一级的前缀：根据当前前缀的最后4字符决定是加 "│   " 还是 "    "
        std::wstring basePrefix = prefix;
        if (basePrefix.size() >= 4) {
            std::wstring lastBlock = basePrefix.substr(basePrefix.size() - 4);
            basePrefix.resize(basePrefix.size() - 4);
            basePrefix += (lastBlock == U_LAST) ? U_SPACE : U_PIPE;
        }

        for (size_t i = 0; i < node.children.size(); ++i) {
            bool isLast = (i == node.children.size() - 1);
            RenderTextRecursive(node.children[i],
                basePrefix + (isLast ? U_LAST : U_BRANCH),
                writer, false);
        }
    }
}

// ============================================================================
// [Part 3] HTML 格式生成器 (Render HTML)
// ============================================================================

/// @brief 对宽字符串进行 HTML 转义（防 XSS）
/// @param wstr 原始宽字符串
/// @return 转义后的 UTF-8 字符串
std::string HtmlEscape(const std::wstring& wstr) {
    std::string str = to_utf8(wstr);
    std::string buffer;
    buffer.reserve(str.size());
    for (char c : str) {
        switch (c) {
        case '&':  buffer.append("&amp;"); break;
        case '\"': buffer.append("&quot;"); break;
        case '\'': buffer.append("&apos;"); break;
        case '<':  buffer.append("&lt;"); break;
        case '>':  buffer.append("&gt;"); break;
        default:   buffer.push_back(c); break;
        }
    }
    return buffer;
}

/// @brief 判断文件是否可预览（点击能打开）
/// @param name 文件名（含扩展名）
/// @return 若支持预览（文本/图片/音视频等）则返回 true
bool IsPreviewable(const std::wstring& name) {
    std::wstring ext = fs::path(name).extension().wstring();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::towlower);
    // 支持常见文本、图片、音视频、PDF
    if (ext == L".txt" || ext == L".cpp" || ext == L".h" || ext == L".c" ||
        ext == L".hpp" || ext == L".java" || ext == L".py" || ext == L".js" ||
        ext == L".html" || ext == L".css" || ext == L".json" || ext == L".xml" ||
        ext == L".md")
        return true;
    if (ext == L".jpg" || ext == L".jpeg" || ext == L".png" || ext == L".gif" ||
        ext == L".bmp" || ext == L".svg" || ext == L".webp")
        return true;
    if (ext == L".mp3" || ext == L".wav" || ext == L".ogg")
        return true;
    if (ext == L".mp4" || ext == L".webm")
        return true;
    if (ext == L".pdf")
        return true;
    return false;
}

/// @brief 递归渲染 HTML 树节点
/// @param node 当前节点
/// @param out 输出流
/// @param isLast 是否为同级最后一个（用于样式）
void RenderHtmlRecursive(const TreeNode& node, std::ostream& out, bool isLast) {
    std::string safeName = HtmlEscape(node.name);
    std::string safePath = HtmlEscape(node.relativePath.wstring());
    std::replace(safePath.begin(), safePath.end(), '\\', '/'); // 统一为正斜杠

    std::string sizeStr = to_utf8(FormatSize(node.size));

    static int idCounter = 0;
    std::string uniqueId = "node_" + std::to_string(++idCounter);

    // 每个 <li> 代表一个节点
    out << "<li data-last=\"" << (isLast ? "true" : "false") << "\">\n";
    out << "  <div class='row' title='Size: " << sizeStr << "' data-name=\"" << safeName << "\">\n";

    // 复选框（用于导出选中项）
    out << "    <input type='checkbox' id='cb_" << uniqueId << "' data-path='" << safePath << "' onclick='toggleCheck(this)'>\n";

    if (node.isDir) {
        // 目录：带折叠三角的可点击区域
        out << "    <span class='caret' data-expanded=\"true\" onclick='toggleFolder(this)'>";
        out << "<span class='file-icon'>📁</span> " << safeName << "</span>\n";
    }
    else {
        // 文件：带图标和链接
        std::string icon = "📄";
        out << "    <span class='file-icon'>" << icon << "</span>";
        if (IsPreviewable(node.name)) {
            out << "<a href='" << safePath << "' target='_blank' class='file-link'>" << safeName << "</a>\n";
        }
        else {
            out << "<a href='#' onclick='return false;' class='file-link disabled'>" << safeName << "</a>\n";
        }
    }

    // 大小徽章
    out << "    <span class='size-badge'>" << sizeStr << "</span>\n";
    out << "  </div>\n";

    // 渲染子节点
    if (node.isDir && !node.children.empty()) {
        out << "  <ul class='nested'>\n"; // 默认展开（CSS 控制折叠）
        for (size_t i = 0; i < node.children.size(); ++i) {
            RenderHtmlRecursive(node.children[i], out, i == node.children.size() - 1);
        }
        out << "  </ul>\n";
    }

    out << "</li>\n";
}

// HTML 模板头部 (CSS)
const char* HTML_HEAD = R"raw(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<style>
    :root {
        --bg-dark: #1e1e1e;
        --bg-panel: #252526;
        --text-primary: #d4d4d4;
        --text-secondary: #9e9e9e;
        --accent-blue: #4fc1ff;
        --accent-green: #60c060;
        --border-color: #3c3c3c;
        --hover-bg: #2a2d2e;
    }
    body {
        font-family: 'Consolas', 'Courier New', monospace;
        background: var(--bg-dark);
        color: var(--text-primary);
        padding: 20px;
        margin: 0;
    }
    h2 {
        color: var(--accent-blue);
        margin-bottom: 15px;
        font-weight: 600;
    }
    .toolbar {
        display: flex;
        gap: 12px;
        margin-bottom: 15px;
        flex-wrap: wrap;
    }
    .search-box {
        padding: 8px 12px;
        background: var(--bg-panel);
        border: 1px solid var(--border-color);
        border-radius: 4px;
        color: var(--text-primary);
        font-family: inherit;
        width: 300px;
        min-width: 200px;
    }
    .search-box:focus {
        outline: none;
        border-color: var(--accent-blue);
    }
    .btn {
        padding: 8px 15px;
        background: var(--accent-green);
        color: #000;
        border: none;
        border-radius: 4px;
        cursor: pointer;
        font-size: 14px;
        font-family: inherit;
        font-weight: bold;
    }
    .btn:hover {
        background: #55bb55;
    }
    .tree-container {
        background: var(--bg-panel);
        padding: 20px;
        border-radius: 8px;
        box-shadow: 0 4px 12px rgba(0,0,0,0.3);
        border: 1px solid var(--border-color);
    }
    ul, #myUL {
        list-style-type: none;
        padding-left: 20px;
    }
    #myUL {
        padding-left: 0;
    }
    .nested.collapsed {
        display: none !important;
    }
    .caret {
        cursor: pointer;
        user-select: none;
        color: var(--accent-blue);
        display: inline-flex;
        align-items: center;
        gap: 6px;
    }
    .caret::before {
        content: "▶";
        transition: transform 0.2s;
        font-size: 0.9em;
    }
    .caret-down::before {
        transform: rotate(90deg);
    }

    /* 行 */
    .row {
        display: flex;
        align-items: center;
        padding: 5px 0;
        transition: background 0.2s;
    }
    .row:hover {
        background: var(--hover-bg);
        border-radius: 4px;
    }

    /* Checkbox */
    input[type=checkbox] {
        margin-right: 10px;
        cursor: pointer;
        accent-color: var(--accent-green);
    }

    /* 文件/文件夹图标 */
    .file-icon {
        margin-right: 6px;
        opacity: 0.8;
    }

    /* 链接 */
    a {
        color: var(--text-primary);
        text-decoration: none;
        margin-left: 4px;
    }
    a:hover {
        color: var(--accent-blue);
        text-decoration: underline;
    }
    a.disabled {
        color: var(--text-secondary);
        cursor: default;
        text-decoration: none;
    }
    a.disabled:hover {
        color: var(--text-secondary);
    }

    /* Size badge */
    .size-badge {
        margin-left: auto;
        font-size: 0.85em;
        color: var(--text-secondary);
        background: #333;
        padding: 2px 8px;
        border-radius: 4px;
        margin-left: 12px;
    }

    /* 搜索高亮 */
    .highlight {
        background-color: #444400;
        padding: 0 2px;
        border-radius: 2px;
    }
</style>
</head>
<body>
    <h2>Project Directory Tree</h2>
    <div class="toolbar">
        <input type="text" id="searchBox" class="search-box" placeholder="Search files/folders...">
        <button class="btn" type="button" onclick="exportSelectedAsTree()">Export Selected Tree (.txt)</button>
    </div>
    <div class="tree-container">
        <ul id="myUL">
)raw";

const char* HTML_TAIL = R"raw(
        </ul>
    </div>

<script>
/**
 * 状态定义 (State Definition):
 * 0: 未选 (Unchecked)
 * 1: 半选 (Partial / Indeterminate) - 视为选中 (Selected)
 * 2: 全选 (Checked) - 视为选中 (Selected)
 */

// 获取状态
function getState(cb) {
    return parseInt(cb.dataset.state || "0");
}

// 设置状态 (同时更新UI)
function setState(cb, state) {
    cb.dataset.state = state;
    if (state === 0) {
        cb.checked = false;
        cb.indeterminate = false;
    } else if (state === 1) {
        cb.checked = true;       // 逻辑上是选中
        cb.indeterminate = true; // 视觉上是减号
    } else if (state === 2) {
        cb.checked = true;
        cb.indeterminate = false; // 视觉上是勾选
    }
}

// UI: 折叠文件夹
function toggleFolder(caret) {
    const li = caret.closest('li');
    if (!li) return;
    const nested = li.querySelector('.nested');
    if (nested) {
        nested.classList.toggle('collapsed');
        caret.classList.toggle('caret-down');
    }
}

// 核心逻辑: 向上传播 (Bottom-Up)
// 规则 4: 任一子项选中 -> 父级半选
// 规则 5: 本级子项全选中 -> 父级全选
// 规则 6: 半选也是选中 (所以统计时 1 和 2 都算选中)
function updateAncestors(li) {
    let parentLi = li.parentElement.closest('li');
    while (parentLi) {
        const parentCb = parentLi.querySelector('input[type="checkbox"]');
        const nestedUl = parentLi.querySelector('.nested');
        if (!nestedUl || !parentCb) break;

        // 获取所有可见子项的 checkbox
        const childrenCbs = Array.from(nestedUl.children)
            .filter(child => child.style.display !== 'none')
            .map(child => child.querySelector('input[type="checkbox"]'))
            .filter(cb => cb);

        const total = childrenCbs.length;
        if (total === 0) break;

        let selectedCount = 0; // 状态为 1 或 2
        
        childrenCbs.forEach(cb => {
            const s = getState(cb);
            if (s > 0) selectedCount++;
        });

        // 计算新状态
        let newState = 0;
        if (selectedCount === 0) {
            newState = 0; // 规则 2 (逆向): 子项全未选 -> 父级未选 (或保持)
        } else if (selectedCount === total) {
            newState = 2; // 规则 5: 本级子项全选中 -> 父级全选
        } else {
            newState = 1; // 规则 4: 任一子项选中(但非全部) -> 父级半选
        }

        // 如果状态没变，就不用继续往上找了
        if (getState(parentCb) === newState) break;

        setState(parentCb, newState);
        parentLi = parentLi.parentElement.closest('li');
    }
}

// 核心逻辑: 点击处理 (Click Handler)
// 这里的 logic 必须用 setTimeout 0 或者 preventDefault 来对抗浏览器原生的 checkbox 行为
window.toggleCheck = function(source) {
    // 将逻辑推迟到微任务，确保覆盖浏览器的默认行为
    setTimeout(() => {
        const cb = source;
        const li = cb.closest('li');
        const isDir = !!li.querySelector('.caret');
        const oldState = getState(cb);
        
        // --- 1. 计算下一状态 (自由切换循环) ---
        let nextState = 0;
        if (isDir) {
            // 文件夹循环: 0(未) -> 1(半) -> 2(全) -> 0(未)
            nextState = (oldState + 1) % 3;
        } else {
            // 文件循环: 0(未) -> 2(全) -> 0(未)
            nextState = (oldState === 0) ? 2 : 0;
        }

        // --- 2. 应用自身状态 ---
        setState(cb, nextState);

        // --- 3. 向下传播 (Top-Down) ---
        if (isDir) {
            const nested = li.querySelector('.nested');
            if (nested) {
                const childrenLis = Array.from(nested.children);
                childrenLis.forEach(childLi => {
                    const childCb = childLi.querySelector('input[type="checkbox"]');
                    const childIsDir = !!childLi.querySelector('.caret');

                    // 规则 2: 父项未选(0) -> 一级子项全未选
                    if (nextState === 0) {
                        setState(childCb, 0);
                        // 如果子项是文件夹，它变0了，它的子项需要递归变0吗？
                        // 通常"未选"是强行清空的。为了保证彻底清空，建议递归。
                        // 但根据规则文字 "父项未选->一级子项全都是未选"，我们只处理一级。
                        // *补充*: 如果一级子项是文件夹，它变成0后，它的子项保持不变？这会导致逻辑矛盾（父0子1）。
                        // 所以这里其实应该递归清空，或者利用 updateAncestors 自动修正？
                        // 为了简单且符合直觉：父级关掉，子级全部关掉。
                        if (childIsDir) clearRecursive(childCb);
                    }
                    // 规则 1: 父项全选(2) -> 一级子项选中 (文件夹半选，文件全选)
                    else if (nextState === 2) {
                        if (childIsDir) {
                            setState(childCb, 1); // 文件夹 -> 半选
                            // 规则 3: 父项半选 -> 子项不变 (所以这里不动孙子节点)
                        } else {
                            setState(childCb, 2); // 文件 -> 全选
                        }
                    }
                    // 规则 3: 父项半选(1) -> 子项不变
                    else if (nextState === 1) {
                        // Do nothing
                    }
                });
            }
        }

        // --- 4. 向上传播 (Bottom-Up) ---
        updateAncestors(li);

    }, 0);
};

// 辅助: 递归清空 (用于父级变0时，彻底清空子树)
function clearRecursive(cb) {
    setState(cb, 0);
    const li = cb.closest('li');
    const nested = li.querySelector('.nested');
    if (nested) {
        nested.querySelectorAll('input[type="checkbox"]').forEach(child => {
            setState(child, 0);
        });
    }
}

// 初始化
document.addEventListener("DOMContentLoaded", function() {
    // 1. 初始化 data-state
    document.querySelectorAll('input[type="checkbox"]').forEach(cb => {
        // 默认全0
        cb.dataset.state = "0";
        cb.checked = false;
        cb.indeterminate = false;
    });

    // 2. 展开所有文件夹
    document.querySelectorAll('.caret').forEach(c => c.classList.add('caret-down'));

    // 3. 搜索功能
    document.getElementById('searchBox').addEventListener('input', function(e) {
        const term = e.target.value.trim().toLowerCase();
        const ul = document.getElementById('myUL');
        
        if (!term) {
            ul.querySelectorAll('li').forEach(li => li.style.display = '');
            return;
        }

        const filter = (li) => {
            let isVisible = false;
            const row = li.querySelector('.row');
            const name = row ? (row.getAttribute('data-name') || '').toLowerCase() : '';
            // 自身匹配
            if (name.includes(term)) isVisible = true;

            // 子项匹配
            const nested = li.querySelector('.nested');
            if (nested) {
                Array.from(nested.children).forEach(childLi => {
                    if (filter(childLi)) isVisible = true;
                });
            }
            li.style.display = isVisible ? '' : 'none';
            return isVisible;
        };

        Array.from(ul.children).forEach(li => filter(li));
    });
});

// 导出功能
function buildTextTreeFromDOM(ul, prefix = '', isRoot = true) {
    if (!ul || !ul.children.length) return [];
    const lines = [];

    // 只处理可见的
    const children = Array.from(ul.children).filter(li => li.style.display !== 'none');

    for (let i = 0; i < children.length; i++) {
        const li = children[i];
        const cb = li.querySelector('input[type="checkbox"]');
        const state = getState(cb);
        
        // 规则 6: 半选也是选中
        const isSelected = (state > 0);

        const row = li.querySelector('.row');
        const name = row.getAttribute('data-name') || 'Unknown';
        const isDir = !!row.querySelector('.caret');
        
        const isLast = (i === children.length - 1);
        const currentPrefix = isRoot ? '' : (isLast ? '└── ' : '├── ');
        const fullLine = isRoot ? (name + '\\') : (prefix + currentPrefix + name + (isDir ? '\\' : ''));

        if (isSelected) {
            lines.push(fullLine);
        }

        const nested = li.querySelector('.nested');
        if (nested && isSelected) { // 只有父级选中(含半选)才递归
            const nextPrefix = isRoot ? '' : (isLast ? prefix + '    ' : prefix + '│   ');
            const childLines = buildTextTreeFromDOM(nested, nextPrefix, false);
            lines.push(...childLines);
        }
    }
    return lines;
}

function exportSelectedAsTree() {
    let hasSelection = false;
    document.querySelectorAll('input[type="checkbox"]').forEach(cb => {
        if (getState(cb) > 0) hasSelection = true;
    });

    if (!hasSelection) {
        alert('No items selected.');
        return;
    }

    const lines = buildTextTreeFromDOM(document.getElementById('myUL'));
    if (lines.length === 0) {
        alert('No visible selected items to export.');
        return;
    }

    const content = lines.join('\r\n') + '\r\n';
    const blob = new Blob([content], { type: 'text/plain;charset=utf-8' });
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = 'directory_tree.txt';
    document.body.appendChild(a);
    a.click();
    document.body.removeChild(a);
    URL.revokeObjectURL(url);
}
</script>
</body>
</html>
)raw";

// ============================================================================
// [Part 4] 统一入口
// ============================================================================

void GenerateTree(
    const fs::path& inputDir,
    const fs::path& outputPath,
    const TreeIgnore& ignore,
    OutputFormat format,
    std::wstringstream* clipboardBuffer,
    bool consoleMode
) {
    // 1. 仅在文件模式下打印处理日志，避免污染控制台输出
    if (!consoleMode) {
        std::cout << to_utf8(Strings::get("PROCESSING")) << std::endl;
    }

    // 2. 构建内存树
    TreeNode root;
    BuildTreeRecursive(inputDir, inputDir, ignore, root);

    // 修正根节点名称显示
    if (root.name == L".") {
        std::error_code ec;
        root.name = fs::absolute(inputDir, ec).filename().wstring();
    }

    // 3. 分发输出逻辑
    if (consoleMode) {
        // ========== 控制台模式 (Case 1 & 2) ==========
        // 直接输出内容到 std::cout，不写文件，不打印 "Saved"

        if (format == OutputFormat::HTML) {
            std::cout << HTML_HEAD;
            RenderHtmlRecursive(root, std::cout, false);
            std::cout << HTML_TAIL;
        }
        else {
            // TEXT 格式：直接利用 MultiWriter 输出到 cout
            MultiWriter writer;
            writer.setConsole(std::cout); // 仅绑定 Console

            if (clipboardBuffer) {
                writer.setClipboard(*clipboardBuffer);
            }

            // Console 不需要 BOM，通常环境已设为 UTF-8
            RenderTextRecursive(root, L"", writer, true);
        }
    }
    else {
        // ========== 文件模式 (Case 3 & 4) ==========
        // 必须写入文件

        // 确定文件名
        fs::path finalPath = outputPath;
        if (finalPath.empty()) {
            // 获取当前时间
            auto now = std::chrono::system_clock::now();
            auto time_t_now = std::chrono::system_clock::to_time_t(now);

            // 安全地转换为 std::tm（本地时间）
            std::tm tm{};
#if defined(_MSC_VER) || defined(_WIN32)
            localtime_s(&tm, &time_t_now);  // Windows 安全函数
#else
            localtime_r(&time_t_now, &tm);  // POSIX
#endif

            std::wstringstream wss;
            wss << L"tree_" << std::put_time(&tm, L"%Y%m%d_%H%M%S");
            wss << (format == OutputFormat::HTML ? L".html" : L".txt");
            finalPath = fs::current_path() / wss.str();
        }

        std::ofstream outFile(finalPath, std::ios::binary);
        if (!outFile.is_open()) {
            std::cerr << to_utf8(Strings::get("ERR_FILE_OPEN") + finalPath.wstring()) << std::endl;
            return;
        }

        if (format == OutputFormat::HTML) {
            outFile << HTML_HEAD;
            RenderHtmlRecursive(root, outFile, false);
            outFile << HTML_TAIL;
        }
        else {
            // TEXT 格式：写入文件 + (可选)剪贴板
            // 注意：文件模式下，这里通常不在控制台重复打印树的内容，只打印 "Saved..."

            outFile << "\xEF\xBB\xBF"; // UTF-8 BOM

            MultiWriter writer;
            writer.setFile(outFile); // 绑定文件

            // 注意：此处不再 writer.setConsole(std::cout)，否则会在生成文件的同时刷屏

            if (clipboardBuffer) {
                writer.setClipboard(*clipboardBuffer);
            }

#ifdef _WIN32
            writer.setLineEndingToCRLF();
#endif

            RenderTextRecursive(root, L"", writer, true);
        }

        outFile.close();

        // 成功日志
        std::cout << to_utf8(Strings::get("MSG_SAVED")) << to_utf8(finalPath.wstring()) << std::endl;
    }
}
