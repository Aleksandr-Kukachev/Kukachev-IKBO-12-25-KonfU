#include "ScriptRunner.h"
ScriptResult ScriptRunner::run(std::istream& input, const std::function<Result(const std::string&)>& submit) {
    std::string line;
    size_t n = 0;
    while (std::getline(input, line)) {
        ++n;
        if (n == 1 && line.rfind("\xEF\xBB\xBF", 0) == 0) line.erase(0, 3);
        if (!line.empty() && line.back() == '\r') line.pop_back();
        auto first = line.find_first_not_of(" \t");
        if (first == std::string::npos || line[first] == '#') continue;
        auto result = submit(line);
        if (!result.ok) return {false, false, n, "script stopped at line " + std::to_string(n)};
        if (result.exit) return {true, true, n, {}};
    }
    if (input.bad()) return {false, false, n, "script read error"};
    return {true, false, n, {}};
}
