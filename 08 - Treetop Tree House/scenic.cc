#include <fmt/format.h>
#include <fstream>
#include <ranges>
#include <string>
#include <vector>

using std::views::iota;

int64_t scoreUp(const auto &field, [[maybe_unused]] auto h,
                [[maybe_unused]] auto w, auto x, auto y) {
    const auto tree = field[y][x];
    int64_t score = 0;
    --y;
    while (y >= 0) {
        ++score;
        if (field[y][x] >= tree) {
            break;
        }
        --y;
    }
    return score;
}

int64_t scoreDown(const auto &field, [[maybe_unused]] auto h,
                  [[maybe_unused]] auto w, auto x, auto y) {
    const auto tree = field[y][x];
    int64_t score = 0;
    ++y;
    while (y < h) {
        ++score;
        if (field[y][x] >= tree) {
            break;
        }
        ++y;
    }
    return score;
}

int64_t scoreLeft(const auto &field, [[maybe_unused]] auto h,
                  [[maybe_unused]] auto w, auto x, auto y) {
    const auto tree = field[y][x];
    int64_t score = 0;
    --x;
    while (x >= 0) {
        ++score;
        if (field[y][x] >= tree) {
            break;
        }
        --x;
    }
    return score;
}

int64_t scoreRight(const auto &field, [[maybe_unused]] auto h,
                   [[maybe_unused]] auto w, auto x, auto y) {
    const auto tree = field[y][x];
    int64_t score = 0;
    ++x;
    while (x < w) {
        ++score;
        if (field[y][x] >= tree) {
            break;
        }
        ++x;
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

    int64_t score = 0;
    int64_t mx = 0, my = 0;
    for (const auto x : iota(0, width)) {
        for (const auto y : iota(0, height)) {
            const auto localScore = scoreUp(field, height, width, x, y) *
                                    scoreDown(field, height, width, x, y) *
                                    scoreLeft(field, height, width, x, y) *
                                    scoreRight(field, height, width, x, y);
            if (score < localScore) {
                mx = x;
                my = y;
                score = localScore;
            }
        }
    }

    fmt::print("Build the treehouse at ({}, {}) {}/10 scenic score\n", mx, my,
               score);
}
