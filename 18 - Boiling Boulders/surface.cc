#include <algorithm>
#include <fmt/format.h>
#include <fstream>
#include <iostream>
#include <ranges>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "simpleparser.hpp"
#include "vec3.hpp"

using std::views::iota;

std::vector<Vec3l> directions{{-1, 0, 0}, {1, 0, 0},  {0, -1, 0},
                              {0, 1, 0},  {0, 0, -1}, {0, 0, 1}};

std::unordered_set<Vec3l> lava{};
std::unordered_set<Vec3l> water{};

void flood(const Vec3l &min, const Vec3l &max) {
    std::unordered_set<Vec3l> toFill{};
    toFill.insert(min);
    while (!toFill.empty()) {
        auto it = toFill.begin();
        const auto current = *it;
        toFill.erase(it);
        water.insert(current);
        for (const auto &dir : directions) {
            const auto next = current + dir;
            if (!water.contains(next) and !lava.contains(next)) {
                if (next.x >= min.x and next.y >= min.y and next.z >= min.z) {
                    if (next.x <= max.x and next.y <= max.y and
                        next.z <= max.z) {
                        toFill.insert(next);
                    }
                }
            }
        }
    }
}

void flood() {
    const auto [min, max] = boundingBox(lava);
    flood(min - Vec3l{1, 1, 1}, max + Vec3l{1, 1, 1});
}

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input.txt>\n";
        std::exit(EXIT_FAILURE);
    }

    SimpleParser scanner{argv[1]};
    while (!scanner.isEof()) {
        const auto x = scanner.getInt64();
        scanner.skipChar(',');
        const auto y = scanner.getInt64();
        scanner.skipChar(',');
        const auto z = scanner.getInt64();
        lava.emplace(x, y, z);
    }
    int64_t outer = 0;
    for (const auto &drop : lava) {
        for (const auto &dir : directions) {
            if (!lava.contains(drop + dir)) {
                ++outer;
            }
        }
    }
    fmt::print("The outside is {} square units\n", outer);

    flood();
    outer = 0;
    for (const auto &drop : lava) {
        for (const auto &dir : directions) {
            if (water.contains(drop + dir)) {
                ++outer;
            }
        }
    }
    fmt::print("The reachable outside is {} square units\n", outer);
}
