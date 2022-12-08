#include <fmt/format.h>
#include <fstream>
#include <ranges>
#include <string>
#include <vector>

#include "scanlocations.hpp"
#include "vec2.hpp"

using std::views::iota;

int64_t score(const auto &field, const Vec2l field_limit, Vec2l position,
              const Vec2l direction) {
    const auto tree = field[position.y][position.x];
    int64_t score = 0;
    position += direction;
    for (const auto look :
         ScanLocations(position, direction, {0, 0}, field_limit)) {
        ++score;
        if (field[look.y][look.x] >= tree) {
            break;
        }
    }
    return score;
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

    int64_t minScore = 0;
    int64_t mx = 0, my = 0;
    for (const auto x : iota(0, width)) {
        for (const auto y : iota(0, height)) {
            const auto localScore = score(field, limit, {x, y}, {0, -1}) *
                                    score(field, limit, {x, y}, {0, 1}) *
                                    score(field, limit, {x, y}, {-1, 0}) *
                                    score(field, limit, {x, y}, {1, 0});
            if (minScore < localScore) {
                mx = x;
                my = y;
                minScore = localScore;
            }
        }
    }

    fmt::print("Build the treehouse at ({}, {}) {}/10 scenic score\n", mx, my,
               minScore);
}
