#include <fmt/format.h>
#include <fstream>
#include <ranges>
#include <string>
#include <vector>

using std::views::iota;

bool seeUp(const auto &field, [[maybe_unused]] auto h, [[maybe_unused]] auto w,
           auto x, auto y) {
    const auto tree = field[y][x];
    --y;
    while (y >= 0) {
        if (field[y][x] >= tree) {
            return false;
        }
        --y;
    }
    return true;
}

bool seeDown(const auto &field, [[maybe_unused]] auto h,
             [[maybe_unused]] auto w, auto x, auto y) {
    const auto tree = field[y][x];
    ++y;
    while (y < h) {
        if (field[y][x] >= tree) {
            return false;
        }
        ++y;
    }
    return true;
}

bool seeLeft(const auto &field, [[maybe_unused]] auto h,
             [[maybe_unused]] auto w, auto x, auto y) {
    const auto tree = field[y][x];
    --x;
    while (x >= 0) {
        if (field[y][x] >= tree) {
            return false;
        }
        --x;
    }
    return true;
}

bool seeRight(const auto &field, [[maybe_unused]] auto h,
              [[maybe_unused]] auto w, auto x, auto y) {
    const auto tree = field[y][x];
    ++x;
    while (x < w) {
        if (field[y][x] >= tree) {
            return false;
        }
        ++x;
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

    int64_t visible = 0;
    for (const auto x : iota(0, width)) {
        for (const auto y : iota(0, height)) {
            if (seeUp(field, height, width, x, y) or
                seeDown(field, height, width, x, y) or
                seeLeft(field, height, width, x, y) or
                seeRight(field, height, width, x, y)) {
                ++visible;
            }
        }
    }

    fmt::print("There are {} trees visible\n", visible);
}
