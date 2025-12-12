#pragma once
#include "Common.h"

std::string to_utf8(const std::wstring& wstr);
std::wstring to_wide(const std::string& str);

void CopyToClipboardW(const std::wstring& wContent);
void CopyToClipboard(const std::string& utf8Content);
std::string format_size(uintmax_t size);

class MultiWriter {
    std::ostream* _console = nullptr;
    std::ostream* _file = nullptr;
    std::wstringstream* _clipboard = nullptr;
    std::string _lineEnding = "\n";

public:
    void setConsole(std::ostream& os);
    void setFile(std::ostream& os);
    void setClipboard(std::wstringstream& ss);
    void setLineEndingToCRLF();
    void writeLine(const std::wstring& wLine);
};