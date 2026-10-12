// Optional console-only harness for verifying the same core without Qt.
#include "Shell.h"
#include "ScriptRunner.h"
#include <fstream>
#include <iostream>
#include <filesystem>
int main(int argc, char** argv) {
    try {
        std::vector<std::string> args;
        for (int i = 1; i < argc; ++i) args.emplace_back(argv[i]);
        Config config = Config::parse(args);
        if (config.help) { std::cout << Config::usage() << '\n'; return 0; }
        Shell shell(config);
        auto submit = [&](const std::string& line) {
            std::cout << shell.cwd() << "$ " << line << '\n';
            auto r = shell.execute(line);
            if (!r.output.empty()) std::cout << r.output << (r.output.back() == '\n' ? "" : "\n");
            return r;
        };
        std::cout << config.dump() << '\n';
        if (!config.scriptPath.empty()) {
            std::ifstream file(std::filesystem::u8path(config.scriptPath));
            if (!file) { std::cerr << "cannot open script\n"; return 1; }
            auto r = ScriptRunner::run(file, submit);
            if (!r.ok) std::cout << r.error << '\n';
            return r.ok ? 0 : 1;
        }
        std::string line;
        while (std::getline(std::cin, line)) if (submit(line).exit) break;
    } catch (const std::exception& e) { std::cerr << e.what() << '\n'; return 1; }
}
