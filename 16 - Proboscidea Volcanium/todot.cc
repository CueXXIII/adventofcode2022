#include <algorithm>
#include <fmt/format.h>
#include <fstream>
#include <iostream>
#include <map>
#include <ranges>
#include <set>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "simpleparser.hpp"

using std::views::iota;

struct Room {
    std::string name;
    std::vector<std::string> tunnels;
    int64_t pressure;

    // highest pressure first
    auto operator<=>(const Room &other) {
        if (other.pressure == pressure) {
            return name <=> other.name;
        }
        return other.pressure <=> pressure;
    }
};

std::unordered_map<std::string, Room> cave{};

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input.txt>\n";
        std::exit(EXIT_FAILURE);
    }

    SimpleParser scanner{argv[1]};
    while (!scanner.isEof()) {
        scanner.skipToken("Valve");
        const auto name = scanner.getToken();
        scanner.skipToken("has flow rate=");
        const auto pressure = scanner.getInt64();
        if (!scanner.skipToken("; tunnel leads to valve")) {
            scanner.skipToken("; tunnels lead to valves");
        }
        decltype(Room::tunnels) dst{};
        do {
            dst.push_back(scanner.getToken(','));
        } while (scanner.skipChar(','));
        cave.emplace(name, Room{name, dst, pressure});
    }

    fmt::print("graph cave {{\n");
    fmt::print(" overlap=false;\n");
    fmt::print(" splines=true;\n");
    for (const auto &[_, room] : cave) {
        fmt::print(
            " \"{}\" [shape=circle{}{} label=\"{}\\n{}\"];\n", room.name,
            (room.name == "AA" ? ",peripheries=2" : ""),
            ((room.name != "AA" and room.pressure == 0) ? ",style=dotted" : ""),
            room.name, room.pressure);
    }
    for (const auto &[_, room] : cave) {
        for (const auto &dest : room.tunnels) {
            if (dest > room.name) {
                fmt::print(" {} -- {};\n", room.name, dest);
            }
        }
    }
    fmt::print("}}\n");
}
