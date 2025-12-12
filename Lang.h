#pragma once
#include "Common.h"

enum class Lang { CN, EN };

Lang detect_system_language();

struct Strings {
    static std::wstring get(const std::string& key);
};