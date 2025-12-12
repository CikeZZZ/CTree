#pragma once
#include "Common.h"

class TreeIgnore {
    struct Rule {
        std::wstring pattern;
        bool onlyDir;
        bool isRootOnly;
        bool hasSeparator;
    };
public:
    std::vector<Rule> rules;
    fs::path rootPath;

    void set_root(const fs::path& path);
    void add_rule(std::wstring raw);
    void load_file(const fs::path& path);
    bool should_ignore(const fs::path& fullPath, bool isDirectory) const;
};

fs::path get_global_ignore_path();
void create_ignore_template(const fs::path& path);