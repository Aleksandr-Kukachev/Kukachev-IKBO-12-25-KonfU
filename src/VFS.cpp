#include "VFS.h"
#include <fstream>
#include <filesystem>
#include <stdexcept>
#include <utility>

namespace {
// CSV records support quoted fields, escaped double quotes, CRLF and quoted newlines.
bool readRow(std::istream& in, std::vector<std::string>& cells) {
    cells.clear();
    std::string field;
    bool quoted = false, closed = false, seen = false;
    char c;
    while (in.get(c)) {
        seen = true;
        if (quoted) {
            if (c == '"') {
                if (in.peek() == '"') { in.get(c); field += '"'; }
                else { quoted = false; closed = true; }
            } else field += c;
            continue;
        }
        if (c == ',' || c == '\n' || c == '\r') {
            cells.push_back(field); field.clear(); closed = false;
            if (c != ',') {
                if (c == '\r' && in.peek() == '\n') in.get(c);
                return true;
            }
        } else if (c == '"') {
            if (closed || !field.empty()) throw std::runtime_error("invalid CSV quote");
            quoted = true;
        } else {
            if (closed) throw std::runtime_error("unexpected character after CSV quote");
            field += c;
        }
    }
    if (in.bad()) throw std::runtime_error("CSV read error");
    if (quoted) throw std::runtime_error("unclosed CSV quote");
    if (seen) cells.push_back(field);
    return seen;
}

std::string decode64(const std::string& s) {
    const std::string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    if (s.size() % 4) throw std::runtime_error("invalid Base64 length");
    std::string result;
    for (size_t i = 0; i < s.size(); i += 4) {
        unsigned int v[4] = {};
        int pad = 0;
        for (size_t j = 0; j < 4; ++j) {
            if (s[i+j] == '=') {
                if (i + 4 != s.size() || j < 2) throw std::runtime_error("invalid Base64 padding");
                ++pad;
            } else {
                auto n = alphabet.find(s[i+j]);
                if (pad || n == std::string::npos) throw std::runtime_error("invalid Base64 character");
                v[j] = static_cast<unsigned int>(n);
            }
        }
        if ((pad == 2 && (v[1] & 15)) || (pad == 1 && (v[2] & 3))) throw std::runtime_error("invalid Base64 padding bits");
        result += static_cast<char>((v[0] << 2) | (v[1] >> 4));
        if (pad < 2) result += static_cast<char>((v[1] << 4) | (v[2] >> 2));
        if (!pad) result += static_cast<char>((v[2] << 6) | v[3]);
    }
    return result;
}

bool canonical(const std::string& path) {
    if (path == "/") return true;
    if (path.empty() || path.front() != '/' || path.back() == '/' || path.find('\0') != std::string::npos) return false;
    size_t start = 1;
    while (start <= path.size()) {
        auto end = path.find('/', start);
        auto part = path.substr(start, end == std::string::npos ? end : end - start);
        if (part.empty() || part == "." || part == "..") return false;
        if (end == std::string::npos) break;
        start = end + 1;
    }
    return true;
}
}

VFS::VFS() { nodes_.emplace("/", Node{true, {}}); }
void VFS::add(const std::string& path, Node node) {
    if (!canonical(path)) throw std::runtime_error("invalid VFS path: " + path);
    if (!nodes_.emplace(path, std::move(node)).second) throw std::runtime_error("duplicate VFS path: " + path);
}
std::string VFS::parent(const std::string& path) {
    auto pos = path.rfind('/');
    return pos == 0 || pos == std::string::npos ? "/" : path.substr(0, pos);
}
VFS VFS::defaults() {
    VFS v;
    v.add("/home", {true, {}});
    v.add("/home/user", {true, {}});
    v.add("/home/user/Documents", {true, {}});
    v.add("/home/user/Documents/readme.txt", {false, "Hello from the default VFS!\n"});
    v.add("/etc", {true, {}});
    v.add("/etc/config.txt", {false, "mode=demo\n"});
    return v;
}
VFS VFS::load(std::istream& input) {
    // Remove UTF-8 BOM before parsing the first field (also works with a quoted header).
    if (input.peek() == 0xEF) {
        char bom[3]; input.read(bom, 3);
        if (input.gcount() != 3 || static_cast<unsigned char>(bom[1]) != 0xBB || static_cast<unsigned char>(bom[2]) != 0xBF)
            throw std::runtime_error("invalid UTF-8 BOM");
    }
    std::vector<std::string> row;
    if (!readRow(input, row)) throw std::runtime_error("empty CSV");
    if (row != std::vector<std::string>{"path", "type", "content"}) throw std::runtime_error("CSV header must be path,type,content");
    VFS v;
    v.nodes_.clear();
    size_t record = 1;
    while (readRow(input, row)) {
        ++record;
        if (row.size() == 1 && row[0].empty()) continue;
        if (row.size() != 3) throw std::runtime_error("CSV record " + std::to_string(record) + ": expected 3 fields");
        if (row[1] == "dir") {
            if (!row[2].empty()) throw std::runtime_error("directory has content: " + row[0]);
            v.add(row[0], {true, {}});
        } else if (row[1] == "file") v.add(row[0], {false, decode64(row[2])});
        else throw std::runtime_error("unknown node type: " + row[1]);
    }
    if (!v.at("/").directory) throw std::runtime_error("root must be a directory");
    for (const auto& entry : v.nodes_) {
        if (entry.first == "/") continue;
        if (!v.at(parent(entry.first)).directory) throw std::runtime_error("parent is not a directory: " + entry.first);
    }
    return v;
}
VFS VFS::loadFile(const std::string& filename) {
    std::ifstream input(std::filesystem::u8path(filename), std::ios::binary);
    if (!input) throw std::runtime_error("cannot open VFS: " + filename);
    return load(input);
}
const Node& VFS::at(const std::string& path) const {
    auto it = nodes_.find(path);
    if (it == nodes_.end()) throw std::runtime_error("no such file or directory: " + path);
    return it->second;
}
std::string VFS::resolve(const std::string& cwd, const std::string& path) const {
    if (path.empty()) throw std::runtime_error("empty path");
    std::string current = path.front() == '/' ? "/" : cwd;
    size_t start = 0;
    while (start < path.size()) {
        auto end = path.find('/', start);
        auto part = path.substr(start, end == std::string::npos ? end : end - start);
        if (!part.empty()) {
            if (!at(current).directory) throw std::runtime_error("not a directory: " + current);
            if (part == "..") current = parent(current);
            else if (part != ".") current = current == "/" ? "/" + part : current + "/" + part;
            at(current);
        }
        if (end == std::string::npos) break;
        start = end + 1;
    }
    if (path.back() == '/' && !at(current).directory) throw std::runtime_error("not a directory: " + current);
    return current;
}
std::vector<std::string> VFS::list(const std::string& path, bool all) const {
    if (!at(path).directory) return {path.substr(path.rfind('/') + 1)};
    std::vector<std::string> result;
    if (all) { result.push_back("."); result.push_back(".."); }
    for (const auto& entry : nodes_) {
        if (entry.first == "/" || parent(entry.first) != path) continue;
        auto name = entry.first.substr(entry.first.rfind('/') + 1);
        if (!all && name.front() == '.') continue;
        result.push_back(name);
    }
    return result;
}
