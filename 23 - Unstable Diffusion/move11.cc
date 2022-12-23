#include <algorithm>
#include <array>
#include <fmt/format.h>
#include <fstream>
#include <iostream>
#include <ranges>
#include <string>
#include <unordered_set>
#include <vector>

#include "vec2.hpp"

using std::views::iota;

namespace Dir {
constexpr Vec2l N = {0, -1};
constexpr Vec2l S = {0, 1};
constexpr Vec2l W = {-1, 0};
constexpr Vec2l E = {1, 0};
constexpr auto NE = N + E;
constexpr auto NW = N + W;
constexpr auto SE = S + E;
constexpr auto SW = S + W;
constexpr std::array<Vec2l, 8> All{N, NE, E, SE, S, SW, W, NW};
constexpr std::array<std::array<Vec2l, 3>, 4> AnyOf{
    {{N, NE, NW}, {S, SE, SW}, {W, NW, SW}, {E, NE, SE}}};
} // namespace Dir

using namespace Dir;

std::unordered_set<Vec2l> map{};
std::unordered_set<Vec2l> moveDest{};
std::unordered_set<Vec2l> moveBlocked{};

struct Elf {
    Vec2l pos{};
    Vec2l moveTo{};

    bool considerMoveDir(const Vec2l &dir, const Vec2l &adj1,
                         const Vec2l &adj2) {
        // fmt::print("{}.considerMoveDir({}, {}, {})\n", pos, dir, adj1, adj2);
        if (!map.contains(pos + dir) and !map.contains(pos + adj1) and
            !map.contains(pos + adj2)) {
            // fmt::print("Moving {} in {}\n", pos, dir);
            moveTo = pos + dir;
            if (moveDest.contains(moveTo)) {
                moveBlocked.insert(moveTo);
            } else {
                moveDest.insert(moveTo);
            }
            return true;
        }
        return false;
    }

    void considerMove(const int64_t step) {
        bool foundElf = false;
        // do nothing rule
        moveTo = pos;
        for (const auto dir : Dir::All) {
            if (map.contains(pos + dir)) {
                foundElf = true;
            }
        }
        if (!foundElf) {
            // fmt::print("not moving {}\n", pos);
            return;
        }
        for (const auto propos : iota(0, 4)) {
            const auto &where = AnyOf[(step + propos) % 4];
            if (considerMoveDir(where[0], where[1], where[2])) {
                return;
            }
        }
    }

    void move() {
        if (!moveBlocked.contains(moveTo)) {
            pos = moveTo;
        }
        map.insert(pos);
    }
};

std::vector<Elf> elves{};

void step(const int64_t no) {
    moveDest.clear();
    moveBlocked.clear();
    for (auto &elf : elves) {
        elf.considerMove(no);
    }
    map.clear();
    for (auto &elf : elves) {
        elf.move();
    }
}

void drawMap(auto &coords) {
    auto [min, max] = boundingBox(coords);
    for (const auto y : iota(min.y - 1, max.y + 2)) {
        for (const auto x : iota(min.x - 1, max.x + 2)) {
            if (coords.contains({x, y})) {
                std::cout << '#';
            } else {
                std::cout << '.';
            }
        }
        std::cout << '\n';
    }
    std::cout << '\n';
}

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input.txt>\n";
        std::exit(EXIT_FAILURE);
    }

    std::ifstream infile{argv[1]};
    std::string line;
    int64_t mapY = 0;
    while (std::getline(infile, line)) {
        for (const auto mapX : iota(0, (int64_t)line.size())) {
            if (line[mapX] == '#') {
                // fmt::print("Found ({}, {})\n", mapX,mapY);
                elves.emplace_back(Vec2l{mapX, mapY});
                map.insert({mapX, mapY});
            }
        }
        ++mapY;
    }
    fmt::print("== Initial State ==\n");
    drawMap(map);
    for (const auto i : iota(0, 10)) {
        step(i);
        fmt::print("== End of Round {} ==\n", i + 1);
        drawMap(map);
    }
    const auto [min, max] = boundingBox(map);
    const auto tiles = (max.x - min.x + 1) * (max.y - min.y + 1) - elves.size();
    fmt::print("The elves cover {} empty tiles\n", tiles);
}
