#include <algorithm>
#include <fmt/format.h>
#include <fstream>
#include <limits>
#include <map>
#include <string>

#include "simpleparser.hpp"

struct Node {
    const std::string name{};
    const bool isDirectory = false;

    Node(const std::string &name, bool dir) : name(name), isDirectory(dir) {}
    virtual size_t getSize() const = 0;
};

class Directory;
class File;

struct File : public Node {
    size_t size{};

    File(const std::string &name, size_t size)
        : Node(name, false), size(size) {}
    virtual size_t getSize() const override { return size; }
};

struct Directory : public Node {
    std::map<std::string, Node *> entries{};
    Directory *parent = nullptr;

    Directory(const std::string &name) : Node(name, true) {}
    virtual size_t getSize() const override {
        size_t total = 0;
        for (const auto &[name, node] : entries) {
            total += node->getSize();
        }
        return total;
    }
    Directory *getDirectory(const std::string name) {
        if (name == "..") {
            return parent != nullptr ? parent : this;
        }
        Node *dst = entries.at(name);
        if (dst->isDirectory) {
            return static_cast<Directory *>(dst);
        } else {
            fmt::print("cd: {}: No such file or directory\n", name);
            return this;
        }
    }
    void insert(File *file) {
        if (entries.contains(file->name)) {
            fmt::print("insert: {}: Node already exists\n", file->name);
        }
        entries[file->name] = file;
    }
    void insert(Directory *dir) {
        if (entries.contains(dir->name)) {
            fmt::print("insert: {}: Node already exists\n", dir->name);
        }
        entries[dir->name] = dir;
        dir->parent = this;
    }
};

void ls(Directory *dir, const std::string prefix = "") {
    fmt::print("{}- {} (dir)\n", prefix, dir->name);
    for (const auto &[_, node] : dir->entries) {
        if (node->isDirectory) {
            ls(static_cast<Directory *>(node), prefix + "  ");
        } else {
            fmt::print("  {}- {} (file, size={})\n", prefix, node->name,
                       node->getSize());
        }
    }
}

size_t sum100k(const Directory *dir) {
    size_t total = 0;
    const size_t current = dir->getSize();
    if (current <= 100000) {
        total += current;
    }
    for (const auto &[_, node] : dir->entries) {
        if (node->isDirectory) {
            total += sum100k(static_cast<Directory *>(node));
        }
    }
    return total;
}

size_t removalCandidate(const Directory *dir, const size_t minSize) {
    size_t min = std::numeric_limits<size_t>::max();
    const size_t current = dir->getSize();
    if (current >= minSize) {
        min = current;
    }
    for (const auto &[_, node] : dir->entries) {
        if (node->isDirectory) {
            min = std::min(
                min, removalCandidate(static_cast<Directory *>(node), minSize));
        }
    }
    return min;
}

int main(int, char **argv) {
    std::ifstream infile{argv[1]};
    SimpleParser shell{infile};

    Directory *root = new Directory("/");
    Directory *cwd = root;

    shell.skipToken("$ cd /");
    while (!shell.isEof()) {
        if (shell.skipChar('$')) {
            const std::string command = shell.getToken();
            if (command == "cd") {
                // cd $dir
                const std::string dir = shell.getToken();
                cwd = cwd->getDirectory(dir);
            } else if (command == "ls") {
                // ls
            } else {
                fmt::print("{}: Unknown command\n", command);
            }
        } else if (shell.skipToken("dir")) {
            // "dir" dirname
            const std::string name = shell.getToken();

            Directory *dir = new Directory(name);
            cwd->insert(dir);
        } else {
            // size filename
            const size_t size = shell.getInt64();
            const std::string name = shell.getToken();

            File *file = new File(name, size);
            cwd->insert(file);
        }
    }
    ls(root);
    fmt::print("\n");
    fmt::print("The sum of small (<=100000) directories is {}\n",
               sum100k(root));

    const size_t free = 70000000 - root->getSize();
    const size_t required = 30000000 - free;

    fmt::print("Delete a directory continig {} bytes\n",
               removalCandidate(root, required));
}
