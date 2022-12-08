#include <fmt/format.h>
#include <fstream>
#include <ranges>
#include <string>
#include <vector>

#include "scanlocations.hpp"
#include "vec2.hpp"

using std::views::iota;

bool see(const auto &field, const Vec2l field_limit, Vec2l position,
         const Vec2l direction) {
    const auto tree = field[position.y][position.x];
    position += direction;
    for (const auto look :
         ScanLocations(position, direction, {0, 0}, field_limit)) {
        if (field[look.y][look.x] >= tree) {
            return false;
        }
    }
    return true;
}

int main(int, char **argv) {
    std::ifstream infile{argv[1]};
    std::string line;
    std::vector<std::string> field;

    while (std::getline(infile, line)) {
        field.push_back(line);
    }

    const int64_t height = (int64_t)field.size();
    const int64_t width = (int64_t)field[0].size();
    const Vec2l limit{width - 1, height - 1};

    int64_t visible = 0;
    for (const auto x : iota(0, width)) {
        for (const auto y : iota(0, height)) {
            if (see(field, limit, {x, y}, {0, -1}) or
                see(field, limit, {x, y}, {0, 1}) or
                see(field, limit, {x, y}, {-1, 0}) or
                see(field, limit, {x, y}, {1, 0})) {
                ++visible;
            }
        }
    }

    fmt::print("There are {} trees visible\n", visible);
}
