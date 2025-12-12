#pragma once
#include "Common.h"

// 定义输出格式枚举
enum class OutputFormat {
    TEXT,
    HTML

};

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

    bool isRebuildMode = false;  // true = 重建模式（inputPath 是文件）
    bool emptyFolder = false;

    fs::path specifiedIgnoreFile;
    std::vector<std::wstring> tempIgnores;

    OutputFormat format = OutputFormat::TEXT;

    void parse(int argc, wchar_t* argv[]);
};