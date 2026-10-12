#pragma once
#include <string>
#include <vector>

class Parser {
public:
    static std::vector<std::string> parse(const std::string& line);
};
