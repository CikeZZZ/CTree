#pragma once

// 定义 Windows 版本
#define WINVER 0x0601
#define _WIN32_WINNT 0x0601  

// Windows Headers
#include <windows.h>
#include <shlwapi.h>
#include <shlobj.h> 

// 标准库
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <string>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <map>
#include <sstream>

// 链接库 (MSVC)
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Advapi32.lib")
#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "User32.lib")
#pragma comment(lib, "Ole32.lib")

namespace fs = std::filesystem;

// 常量定义
const std::wstring PARENT_MENU_NAME = L"CikeZZZ";
const std::wstring MENU_TREE_KEY = L"CTree";
const std::wstring MENU_COPY_KEY = L"CopyContent";
const std::wstring IGNORE_FILENAME = L".treeignore";
const std::wstring VERSION = L"1.1.0";