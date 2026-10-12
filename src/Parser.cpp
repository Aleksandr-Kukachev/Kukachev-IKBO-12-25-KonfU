#include "Parser.h"
#include <cctype>
#include <stdexcept>

std::vector<std::string> Parser::parse(const std::string& line) {
    std::vector<std::string> args;
    std::string token;
    char quote = 0;
    bool started = false;
    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        if (c == '\\' && quote != '\'') {
            if (i + 1 == line.size()) throw std::runtime_error("trailing escape");
            char next = line[i + 1];
            // In double quotes only quote and backslash are escaped here.
            if (quote == '"' && next != '"' && next != '\\') token += c;
            else { token += next; ++i; }
            started = true;
        } else if (quote) {
            if (c == quote) quote = 0;
            else token += c;
        } else if (c == '\'' || c == '"') {
            quote = c;
            started = true;
        } else if (std::isspace(static_cast<unsigned char>(c))) {
            if (started) { args.push_back(token); token.clear(); started = false; }
        } else {
            token += c;
            started = true;
        }
    }
    if (quote) throw std::runtime_error("unclosed quote");
    if (started) args.push_back(token);
    return args;
}
