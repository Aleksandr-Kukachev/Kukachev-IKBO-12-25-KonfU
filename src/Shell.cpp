#include "Shell.h"
#include "Parser.h"
#include <stdexcept>
#include <utility>

Shell::Shell(Config config) : config_(std::move(config)),
    vfs_(config_.vfsPath.empty() ? VFS::defaults() : VFS::loadFile(config_.vfsPath)) {}

Result Shell::execute(const std::string& line) {
    std::string cmd;
    try {
        auto args = Parser::parse(line);
        if (args.empty()) return {};
        cmd = args.front();
        if (cmd == "exit" || cmd == "pwd" || cmd == "conf-dump" || cmd == "help") {
            if (args.size() != 1) throw std::runtime_error("unexpected arguments");
            if (cmd == "exit") return {"Bye!", true, true};
            if (cmd == "pwd") return {cwd_};
            if (cmd == "conf-dump") return {config_.dump()};
            return {"ls [-a] [--] [path]\ncd [--] [path]\npwd\ncat [--] path\nconf-dump\nhelp\nexit"};
        }
        if (cmd == "ls" || cmd == "cd" || cmd == "cat") {
            bool all = false, options = true;
            std::vector<std::string> paths;
            for (size_t i = 1; i < args.size(); ++i) {
                if (options && args[i] == "--") { options = false; continue; }
                if (options && args[i].size() > 1 && args[i].front() == '-') {
                    if (cmd == "ls" && args[i] == "-a") all = true;
                    else throw std::runtime_error("unknown option: " + args[i]);
                } else { paths.push_back(args[i]); options = false; }
            }
            if (paths.size() > 1 || (cmd == "cat" && paths.empty())) throw std::runtime_error("invalid number of arguments");
            auto path = vfs_.resolve(cwd_, paths.empty() ? (cmd == "cd" ? "/" : ".") : paths.front());
            if (cmd == "cd") {
                if (!vfs_.at(path).directory) throw std::runtime_error("not a directory: " + path);
                cwd_ = path;
                return {};
            }
            if (cmd == "cat") {
                const auto& node = vfs_.at(path);
                if (node.directory) throw std::runtime_error("is a directory: " + path);
                return {node.content};
            }
            std::string out;
            for (const auto& name : vfs_.list(path, all)) {
                if (!out.empty()) out += '\n';
                out += name;
            }
            return {out};
        }
        return {cmd + ": command not found", false};
    } catch (const std::exception& e) {
        return {(cmd.empty() ? "error" : cmd) + std::string(": ") + e.what(), false};
    }
}
