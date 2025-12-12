#pragma once
#include "Common.h"

// 从文本文件重建目录结构
// treeFile: CTree 生成的文本文件路径
// destRoot: 目标还原目录 (-o 指定的路径)
void RebuildTreeFromFile(const fs::path& treeFile, const fs::path& destRoot, bool emptyFolder);