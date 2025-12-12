#pragma once
#include "Common.h"
#include "Utils.h"
#include "TreeIgnore.h"
#include "Config.h" // 引入 OutputFormat

// 统一入口
void GenerateTree(
    const fs::path& inputDir,
    const fs::path& outputPath,
    const TreeIgnore& ignore,
    OutputFormat format,
    std::wstringstream* clipboardBuffer = nullptr,
    bool consoleMode = false // 新增参数，默认为 false
);