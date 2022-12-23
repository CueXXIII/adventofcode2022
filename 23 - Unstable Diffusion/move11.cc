#include <algorithm>
#include <array>
#include <cstdlib>
#include <fmt/format.h>
#include <fstream>
#include <iostream>
#include <ranges>
#include <string>
#include <unordered_set>
#include <vector>

#include "vec2.hpp"

// ==== set to true for animation ====
constexpr bool visualize = false;

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

    bool move() {
        bool moved = false;
        if (!moveBlocked.contains(moveTo)) {
            moved = (pos != moveTo);
            pos = moveTo;
        }
        map.insert(pos);
        return moved;
    }
};

std::vector<Elf> elves{};

bool step(const int64_t no) {
    moveDest.clear();
    moveBlocked.clear();
    for (auto &elf : elves) {
        elf.considerMove(no);
    }
    map.clear();
    bool moved = false;
    for (auto &elf : elves) {
        moved |= elf.move();
    }
    return moved;
}

void drawMap(auto &coords) {
    auto [min, max] = boundingBox(coords);
    if constexpr (visualize)
        min = {-12 - 5, -13 - 5};
    for (const auto y : iota(min.y - 1, max.y + 2)) {
        for (const auto x : iota(min.x - 1, max.x + 2)) {
            if (coords.contains({x, y})) {
                if constexpr (visualize)
                    std::cout << "##";
                else
                    std::cout << '#';
            } else {
                if constexpr (visualize)
                    std::cout << "  ";
                else
                    std::cout << '.';
            }
        }
        std::cout << '\n';
    }
    std::cout << '\n';
    // fmt::print("[{}, {}]\n",min,max);
}

void startAnimation() { fmt::print("\x1b[H\x1b[2J\x1b[3J"); }
void startAnimFrame() { fmt::print("\x1b[H"); }
void delayAnimation() { usleep(1000 * 1000 / 60); }
void animateMap(auto &map) {
    startAnimFrame();
    drawMap(map);
    delayAnimation();
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
    // fmt::print("== Initial State ==\n");
    // drawMap(map);
    if constexpr (visualize) {
        startAnimation();
        animateMap(map);
        sleep(2);
    }
    for (const auto i : iota(0, 10)) {
        step(i);
        // fmt::print("== End of Round {} ==\n", i + 1);
        // drawMap(map);
        if constexpr (visualize)
            animateMap(map);
    }
    const auto [min, max] = boundingBox(map);
    const auto tiles = (max.x - min.x + 1) * (max.y - min.y + 1) - elves.size();
    int64_t i = 10;
    while (step(i)) {
        if constexpr (visualize)
            animateMap(map);
        ++i;
    }
    fmt::print("The elves cover {} empty tiles after 10 rounds\n", tiles);
    fmt::print("The elves stood still in round {}\n", i + 1);
}
