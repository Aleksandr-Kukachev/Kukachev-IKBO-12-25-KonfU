#pragma once
#include <string>
#include "Config.h"
#include "VFS.h"

struct Result {
    std::string output;
    bool ok = true;
    bool exit = false;
};

class Shell {
    Config config_;
    VFS vfs_;
    std::string cwd_ = "/";
public:
    explicit Shell(Config config = {});
    Result execute(const std::string& line);
    std::string cwd() const { return cwd_; }
};
