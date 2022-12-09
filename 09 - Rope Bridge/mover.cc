#include <algorithm>
#include <fmt/format.h>
#include <fstream>
#include <iostream>
#include <map>
#include <ranges>
#include <string>
#include <unordered_set>
#include <vector>

#include "simpleparser.hpp"
#include "vec2.hpp"

using std::views::iota;

std::unordered_set<Vec2l> visited{};

const std::map<std::string, Vec2l> directions{
    {"R", {1, 0}}, {"L", {-1, 0}}, {"U", {0, -1}}, {"D", {0, 1}}};

struct Rope {
    Vec2l head{0, 0};
    Vec2l tail{0, 0};

    Rope() { visited.insert(tail); }

    template <typename T> T signum(const T num) {
        return (T{0} < num) - (num < T{0});
    }

    void stepTail() {
        const auto difference = head - tail;
        if (std::abs(difference.x) > 1 or std::abs(difference.y) > 1) {
            const Vec2l moveStep = {signum(difference.x), signum(difference.y)};
            tail += moveStep;
            visited.insert(tail);
        }
    }

    void move(const std::string &directionName, const int64_t steps) {
        const auto &direction = directions.at(directionName);
        for ([[maybe_unused]] const auto _ : iota(0, steps)) {
            head += direction;
            stepTail();
        }
    }
};

void printVisited() {
    for (const auto y : iota(-5, 2)) {
        for (const auto x : iota(-1, 7)) {
            if (visited.contains({x, y})) {
                fmt::print("#");
            } else {
                fmt::print(".");
            }
        }
        fmt::print("\n");
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input.txt>\n";
        std::exit(EXIT_FAILURE);
    }

    Rope rope{};

    SimpleParser scanner{argv[1]};
    while (!scanner.isEof()) {
        const auto direction = scanner.getToken();
        const auto steps = scanner.getInt64();
        rope.move(direction, steps);
    }
    fmt::print("The tail visited {} locations\n", visited.size());
}
