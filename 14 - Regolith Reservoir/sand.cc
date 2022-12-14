#include <algorithm>
#include <fmt/format.h>
#include <fstream>
#include <iostream>
#include <ranges>
#include <string>
#include <unordered_map>
#include <vector>

#include "simpleparser.hpp"
#include "vec2.hpp"

using std::views::iota;

constexpr const Vec2l sandStart{500, 0};

template <typename T> static constexpr T signum(const T num) {
    return (T{0} < num) - (num < T{0});
}

void printGrid(const auto &map) {
    Vec2l minPos{sandStart};
    Vec2l maxPos{sandStart};

    for (const auto &[pos, _] : map) {
        minPos = {std::min(minPos.x, pos.x), std::min(minPos.y, pos.y)};
        maxPos = {std::max(maxPos.x, pos.x), std::max(maxPos.y, pos.y)};
    }

    for (const auto y : iota(minPos.y - 1, maxPos.y + 2)) {
        for (const auto x : iota(minPos.x - 1, maxPos.x + 2)) {
            if (map.contains({x, y})) {
                fmt::print("{}", map.at({x, y}));
            } else if (sandStart == Vec2l{x, y}) {
                fmt::print("+");
            } else {
                fmt::print(".");
            }
        }
        fmt::print("\n");
    }
}

Vec2l scanPoint(SimpleParser &scanner) {
    const auto x = scanner.getInt64();
    scanner.skipChar(',');
    const auto y = scanner.getInt64();
    return {x, y};
}

void drawLine(auto &map, const Vec2l src, const Vec2l dst) {
    const Vec2l dir = {signum(dst.x - src.x), signum(dst.y - src.y)};
    for (Vec2l pos = src; pos != dst; pos += dir) {
        map[pos] = '#';
    }
    map[dst] = '#';
}

// drop one grain of sand and return true, if it falls below minY
bool dropSand(auto &map, const auto maxY) {
    constexpr std::array<Vec2l, 4> sandFalls{{{0, 1}, {-1, 1}, {1, 1}, {0, 0}}};
    auto sandPos = sandStart;
    do {
        const auto fallTo =
            sandPos +
            *std::find_if(sandFalls.begin(), sandFalls.end(), [&](auto &dir) {
                return !map.contains(sandPos + dir);
            });
        if (fallTo == sandPos) {
            map[sandPos] = 'o';
            return sandPos == sandStart;
        }
        sandPos = fallTo;
    } while (sandPos.y <= maxY);
    return sandPos.y >= maxY;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input.txt>\n";
        std::exit(EXIT_FAILURE);
    }

    std::unordered_map<Vec2l, char> cave;
    // cave[sandStart] = '+';
    int64_t maxY = sandStart.y;

    SimpleParser scanner{argv[1]};
    while (!scanner.isEof()) {
        Vec2l src = scanPoint(scanner);
        maxY = std::max(maxY, src.y);
        while (scanner.skipToken("->")) {
            Vec2l dst = scanPoint(scanner);
            maxY = std::max(maxY, dst.y);
            drawLine(cave, src, dst);
            src = dst;
        }
    }
    printGrid(cave);
    fmt::print("Maximum depth is {}\n", maxY);

    int64_t sandCount = 0;
    while (!dropSand(cave, maxY)) {
        ++sandCount;
    }
    printGrid(cave);
    fmt::print("\n");
    fmt::print("{} grains of sand fell into the cave\n", sandCount);

    drawLine(cave, {500 - maxY - 5, maxY + 2}, {500 + maxY + 5, maxY + 2});
    while (!dropSand(cave, maxY + 3)) {
        ++sandCount;
    }
    printGrid(cave);
    fmt::print("\n");
    fmt::print("A total of {} grains fell ontop the floor\n", sandCount + 1);
}
