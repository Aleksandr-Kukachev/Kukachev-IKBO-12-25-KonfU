#include "Parser.h"
#include "Shell.h"
#include "ScriptRunner.h"
#include "VFS.h"
#include <stdexcept>
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <filesystem>

void check(bool ok, const char* label) { if (!ok) throw std::runtime_error(label); }
template<class F> void rejects(F f, const char* label) { bool threw = false; try { f(); } catch (const std::exception&) { threw = true; } check(threw, label); }
int main(int argc, char** argv) {
    try {
        check(Parser::parse("ls \"two words\" 'three words' \"\"") == std::vector<std::string>{"ls", "two words", "three words", ""}, "quotes");
        check(Parser::parse("ls a\\ b pre\"fix\"") == std::vector<std::string>{"ls", "a b", "prefix"}, "escape and join");
        check(Parser::parse("cat \"a\\\"b\"")[1] == "a\"b", "escaped double quote");
        Shell shell;
        check(shell.execute("ls").output == "etc\nhome", "default ls");
        check(shell.execute("cd /home/user/Documents").ok, "cd absolute");
        check(shell.execute("pwd").output == "/home/user/Documents", "pwd");
        check(shell.execute("cat readme.txt").output == "Hello from the default VFS!\n", "cat relative path");
        check(shell.execute("ls ./readme.txt").output == "readme.txt", "ls file");
        check(!shell.execute("cd readme.txt").ok, "not a directory");
        check(!shell.execute("cd missing/..").ok, "invalid intermediate component");
        check(!shell.execute("cat .").ok, "cat directory");
        check(shell.execute("cd ../..").ok && shell.cwd() == "/home", "cd relative");
        check(shell.execute("cd").ok && shell.cwd() == "/", "cd root");
        check(shell.execute("cd ..").ok && shell.cwd() == "/", "root parent");
        check(!shell.execute("ls \"broken").ok, "unclosed quote");
        check(!shell.execute("unknown").ok, "unknown command");
        check(!shell.execute("ls -z").ok, "unknown option");
        check(!shell.execute("exit extra").ok, "exit args");
        check(shell.execute("exit").exit, "exit");
        auto conf = Config::parse({"--vfs", "file name.csv", "--script=start.txt"});
        check(conf.dump() == "vfs=file name.csv\nscript=start.txt", "conf dump");
        check(shell.execute("conf-dump").output == "vfs=(default)\nscript=(none)", "command conf dump");
        rejects([]{ Config::parse({"--vfs"}); }, "missing value");
        rejects([]{ Config::parse({"--unknown"}); }, "unknown parameter");
        rejects([]{ Config::parse({"--vfs=a", "--vfs=b"}); }, "duplicate parameter");
        std::string csv = "path,type,content\n/,dir,\n/d,dir,\n/d/.hidden,file,\n\"/d/a, b\",file,AP8Q\n";
        std::istringstream csvStream(csv);
        auto v = VFS::load(csvStream);
        check(v.resolve("/", "d/./a, b") == "/d/a, b", "quoted csv path");
        check(v.at("/d/a, b").content == std::string("\x00\xff\x10", 3), "binary decode");
        check(v.list("/d") == std::vector<std::string>{"a, b"}, "hidden file");
        check(v.list("/d", true) == std::vector<std::string>{".", "..", ".hidden", "a, b"}, "ls all");
        rejects([]{ std::istringstream s("path,type,content\n/,dir,\n/a/b,file,\n"); VFS::load(s); }, "missing parent");
        rejects([]{ std::istringstream s("path,type,content\n/,dir,\n/a,file,@@@@\n"); VFS::load(s); }, "invalid base64");
        rejects([]{ std::istringstream s("path,type,content\n/,dir,\n/,dir,\n"); VFS::load(s); }, "duplicate root");
        rejects([]{ std::istringstream s("path,type,content\n/,dir,\n/a/../b,file,\n"); VFS::load(s); }, "invalid canonical path");
        rejects([]{ std::istringstream s("path,type,content\n/,dir,\n/a,file,AB==\n"); VFS::load(s); }, "base64 padding bits");
        std::istringstream script("# comment\nls\nunknown\ncd /\n");
        size_t count = 0;
        auto status = ScriptRunner::run(script, [&](const std::string& line) { ++count; return shell.execute(line); });
        check(!status.ok && status.line == 3 && count == 2, "script stops on error");
        std::istringstream exiting("exit\nunknown\n");
        status = ScriptRunner::run(exiting, [&](const std::string& line) { return shell.execute(line); });
        check(status.ok && status.exit && status.line == 1, "script stops on exit");
        if (argc == 2) {
            std::filesystem::path root = argv[1];
            for (auto name : {"minimal", "files", "deep"}) {
                Config cfg; cfg.vfsPath = (root / "data" / (std::string(name)+".csv")).u8string();
                Shell s(cfg);
                check(s.execute("ls").ok, "load fixture");
                if (std::string(name) == "minimal") check(s.execute("ls").output.empty(), "minimal empty");
                if (std::string(name) == "files") check(s.execute("cat 'file with spaces.txt'").ok, "file with spaces");
                if (std::string(name) == "deep") {
                    std::ifstream f(root / "scripts/stage3-start.txt");
                    auto r = ScriptRunner::run(f, [&](const std::string& line){ return s.execute(line); });
                    check(!r.ok && r.line == 21 && s.cwd() == "/", "deep demonstration stops on missing path");
                    for (const auto& entry : std::filesystem::directory_iterator(root / "scripts/errors")) {
                        Shell clean(cfg);
                        std::ifstream errorScript(entry.path());
                        auto result = ScriptRunner::run(errorScript, [&](const std::string& line){ return clean.execute(line); });
                        check(!result.ok && result.line == 2, "error fixture stops at second line");
                    }
                }
            }
        }
        std::cout << "All tests passed\n";
    } catch (const std::exception& e) { std::cerr << e.what() << '\n'; return 1; }
}
