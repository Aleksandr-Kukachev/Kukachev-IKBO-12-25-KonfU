#pragma once
#include "Shell.h"
#include <functional>
#include <istream>
#include <string>
struct ScriptResult { bool ok = true; bool exit = false; size_t line = 0; std::string error; };
class ScriptRunner {
public:
    // Callback echoes the line, executes it and returns its status.
    static ScriptResult run(std::istream& input, const std::function<Result(const std::string&)>& submit);
};
