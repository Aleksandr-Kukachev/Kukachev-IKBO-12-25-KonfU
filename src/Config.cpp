#include "Config.h"
#include <stdexcept>
Config Config::parse(const std::vector<std::string>& args) {
    Config c;
    bool vfsSeen = false, scriptSeen = false;
    for (size_t i = 0; i < args.size(); ++i) {
        const auto arg = args[i];
        if (arg == "--help" || arg == "-h") { c.help = true; continue; }
        auto pos = arg.find('=');
        auto name = arg.substr(0, pos);
        if (name != "--vfs" && name != "--script") throw std::runtime_error("unknown parameter: " + arg);
        std::string value;
        if (pos != std::string::npos) value = arg.substr(pos + 1);
        else {
            if (i + 1 >= args.size() || args[i + 1].rfind("--", 0) == 0) throw std::runtime_error("missing value: " + name);
            value = args[++i];
        }
        if (value.empty()) throw std::runtime_error("empty value: " + name);
        auto& seen = name == "--vfs" ? vfsSeen : scriptSeen;
        if (seen) throw std::runtime_error("duplicate parameter: " + name);
        seen = true;
        (name == "--vfs" ? c.vfsPath : c.scriptPath) = value;
    }
    return c;
}
std::string Config::dump() const {
    return "vfs=" + (vfsPath.empty() ? std::string("(default)") : vfsPath) + "\nscript=" + (scriptPath.empty() ? std::string("(none)") : scriptPath);
}
std::string Config::usage() { return "Usage: shell-emulator [--vfs PATH] [--script PATH] [--help]"; }
