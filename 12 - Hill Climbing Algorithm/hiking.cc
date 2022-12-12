#include <algorithm>
#include <fmt/format.h>
#include <fstream>
#include <iostream>
#include <ranges>
#include <string>
#include <vector>

#include "simpleparser.hpp"
#include "vec2.hpp"

using std::views::iota;

const std::vector<Vec2<size_t>> directions = {
    {-1ULL, 0}, {1, 0}, {0, -1ULL}, {0, 1}};

std::vector<std::string> heightmap;
Vec2<size_t> mapSize;
Vec2<size_t> startPos;
Vec2<size_t> endPos;

std::vector<std::vector<int64_t>> distmap;

void printDistmap() {
    for (const auto y : iota(0u, mapSize.y)) {
        for (const auto x : iota(0u, mapSize.x)) {
            fmt::print("{:4d}", distmap[y][x]);
        }
        fmt::print("\n");
    }
    fmt::print("\n");
}

int64_t fillDistmap() {
    distmap[endPos.y][endPos.x] = 0;
    int64_t step = 1;
    while (true) {
        for (const auto y : iota(0u, mapSize.y)) {
            for (const auto x : iota(0u, mapSize.x)) {
                if (distmap[y][x] != step - 1) {
                    continue;
                }
                const Vec2<size_t> pos{x, y};
                for (const auto &direction : directions) {
                    const auto dst{direction + pos};
                    if (dst.x >= mapSize.x or dst.y >= mapSize.y) {
                        continue;
                    }
                    if (distmap[dst.y][dst.x] != -1) {
                        continue;
                    }
                    if (heightmap[dst.y][dst.x] + 1 >= heightmap[y][x]) {
                        distmap[dst.y][dst.x] = step;
                        if (heightmap[dst.y][dst.x] == 'a') {
                            fmt::print("Hiking start point {}\n", dst);
                            return step;
                        }
                    }
                }
            }
        }
        ++step;
        // printDistmap();
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input.txt>\n";
        std::exit(EXIT_FAILURE);
    }

    std::ifstream infile{argv[1]};
    std::string line;
    size_t y = 0;
    while (std::getline(infile, line)) {
        auto x = line.find('S');
        if (x != std::string::npos) {
            startPos = {x, y};
            line[x] = 'a';
        }
        x = line.find('E');
        if (x != std::string::npos) {
            endPos = {x, y};
            line[x] = 'z';
        }
        heightmap.push_back(line);
        ++y;
        distmap.resize(y);
        distmap[y - 1].resize(line.size(), -1);
    }
    mapSize = {heightmap.front().size(), y};
    fmt::print("map {}\n", mapSize);

    fmt::print("It takes {} steps from the hiking start\n", fillDistmap());
    // printDistmap();
}
