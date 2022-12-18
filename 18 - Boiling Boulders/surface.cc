#include <algorithm>
#include <fmt/format.h>
#include <fstream>
#include <iostream>
#include <ranges>
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
}
