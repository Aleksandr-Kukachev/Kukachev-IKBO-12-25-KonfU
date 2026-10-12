#pragma once
#include <map>
#include <string>
#include <vector>
#include <istream>

struct Node {
    bool directory;
    std::string content; // Decoded bytes, not a physical path.
};

class VFS {
public:
    VFS();
    static VFS defaults();
    static VFS load(std::istream& input);
    static VFS loadFile(const std::string& filename);
    static std::string parent(const std::string& path);
    std::string resolve(const std::string& cwd, const std::string& path) const;
    const Node& at(const std::string& path) const;
    std::vector<std::string> list(const std::string& path, bool all = false) const;
private:
    std::map<std::string, Node> nodes_;
    void add(const std::string& path, Node node);
};
