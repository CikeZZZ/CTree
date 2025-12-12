#include "Utils.h"
#include "Lang.h"

/// @brief 将宽字符串（std::wstring，通常为 UTF-16）转换为 UTF-8 编码的 std::string
/// @param wstr 输入的宽字符串
/// @return 转换后的 UTF-8 字符串；若输入为空，则返回空字符串
std::string to_utf8(const std::wstring& wstr) {
    if (wstr.empty()) return {};
    // 计算所需缓冲区大小（字节数）
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), nullptr, 0, nullptr, nullptr);
    std::string strTo(size_needed, 0); // 预分配缓冲区
    // 执行实际转换
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), &strTo[0], size_needed, nullptr, nullptr);
    return strTo;
}

/// @brief 将 UTF-8 编码的 std::string 转换为宽字符串（std::wstring）
/// @param str 输入的 UTF-8 字符串
/// @return 转换后的宽字符串；若输入为空，则返回空字符串
std::wstring to_wide(const std::string& str) {
    if (str.empty()) return {};
    // 计算所需宽字符数量
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), nullptr, 0);
    std::wstring wstrTo(size_needed, 0); // 预分配缓冲区
    // 执行实际转换
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

/// @brief 将宽字符串内容复制到 Windows 剪贴板（使用 CF_UNICODETEXT 格式）
/// @param wContent 要复制的宽字符串内容（UTF-16）
/// @note 成功后会输出一条本地化提示信息到控制台
void CopyToClipboardW(const std::wstring& wContent) {
    if (wContent.empty()) return;
    if (!OpenClipboard(nullptr)) return; // 打开剪贴板失败则退出
    EmptyClipboard(); // 清空当前剪贴板内容

    // 分配全局内存（包含结尾的 \0）
    size_t size = (wContent.size() + 1) * sizeof(wchar_t);
    HGLOBAL hGlob = GlobalAlloc(GMEM_MOVEABLE, size);
    if (hGlob) {
        void* pLocked = GlobalLock(hGlob); // 锁定内存以写入数据
        if (pLocked) {
            memcpy(pLocked, wContent.c_str(), size); // 复制字符串内容
            GlobalUnlock(hGlob); // 解锁
            // 将数据放入剪贴板；若失败则释放内存
            if (!SetClipboardData(CF_UNICODETEXT, hGlob)) {
                GlobalFree(hGlob);
            }
        }
        else {
            GlobalFree(hGlob); // 锁定失败，释放内存
        }
    }
    CloseClipboard(); // 关闭剪贴板

    // 输出本地化提示（如“已复制到剪贴板”）
    std::cout << to_utf8(Strings::get("MSG_CLIPBOARD")) << std::endl;
}

/// @brief 将 UTF-8 字符串内容复制到剪贴板（内部转换为宽字符串后调用 CopyToClipboardW）
/// @param utf8Content UTF-8 编码的字符串内容
void CopyToClipboard(const std::string& utf8Content) {
    if (utf8Content.empty()) return;
    CopyToClipboardW(to_wide(utf8Content));
}

/// @brief 将字节数格式化为人类可读的字符串（如 1.23 MB）
/// @param size 输入的字节数（uintmax_t 类型，支持大文件）
/// @return 格式化后的字符串，保留两位小数，附带单位（B/KB/MB/GB/TB）
std::string format_size(uintmax_t size) {
    const char* suffixes[] = { "B", "KB", "MB", "GB", "TB" };
    int i = 0;
    double dblSize = (double)size;
    // 逐级除以 1024，直到小于 1024 或达到最大单位 TB
    while (dblSize >= 1024 && i < 4) {
        dblSize /= 1024.0;
        i++;
    }
    char buf[32];
    snprintf(buf, 32, "%.2f %s", dblSize, suffixes[i]);
    return std::string(buf);
}

// MultiWriter 类的成员函数实现

/// @brief 设置输出到控制台的流（如 std::cout）
void MultiWriter::setConsole(std::ostream& os) { _console = &os; }

/// @brief 设置输出到文件的流（如 std::ofstream）
void MultiWriter::setFile(std::ostream& os) { _file = &os; }

/// @brief 设置用于剪贴板输出的宽字符串流（后续可一次性复制）
void MultiWriter::setClipboard(std::wstringstream& ss) { _clipboard = &ss; }

/// @brief 设置行尾为 Windows 风格（CRLF: \r\n），默认是 LF
void MultiWriter::setLineEndingToCRLF() { _lineEnding = "\r\n"; }

/// @brief 同时向控制台、文件、剪贴板流写入一行内容
/// @param wLine 要写入的宽字符串行（不包含换行符）
/// @note 控制台和文件使用 UTF-8 输出，剪贴板保留宽字符并加 L'\n'
void MultiWriter::writeLine(const std::wstring& wLine) {
    std::string u8Line = to_utf8(wLine);
    if (_console) *_console << u8Line << _lineEnding;
    if (_file)    *_file << u8Line << _lineEnding;
    if (_clipboard) *_clipboard << wLine << L'\n';
}