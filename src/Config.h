#pragma once
#include <string>
#include <vector>
struct Config {
    std::string vfsPath, scriptPath;
    bool help = false;
    static Config parse(const std::vector<std::string>& args);
    std::string dump() const;
    static std::string usage();
};
