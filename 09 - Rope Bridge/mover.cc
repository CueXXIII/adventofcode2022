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
#include "utility.hpp"
#include "vec2.hpp"

using std::views::iota;

std::unordered_set<Vec2l> visited1{{0, 0}};
std::unordered_set<Vec2l> visited9{{0, 0}};

static const std::map<std::string, Vec2l> directions{
    {"R", {1, 0}}, {"L", {-1, 0}}, {"U", {0, -1}}, {"D", {0, 1}}};

struct Rope {
    std::vector<Vec2l> knots{{0, 0}};

    Rope() {}

    bool stepTail(const auto &head, auto &tail) {
        const auto difference = head - tail;
        const Vec2l moveStep = signum(difference);
        if (difference != moveStep) {
            tail += moveStep;
            return true;
        }
        return false;
    }

    void move(const std::string &directionName, const int64_t steps) {
        const auto &direction = directions.at(directionName);
        for ([[maybe_unused]] const auto _ : iota(0, steps)) {
            knots[0] += direction;
            for (const auto headNo : iota(0, (int64_t)knots.size() - 1)) {
                if (!stepTail(knots[headNo], knots[headNo + 1])) {
                    break;
                }
            }
            if (knots.back() != Vec2l{0, 0}) {
                knots.emplace_back(0, 0);
            }
            visited1.insert(knots[1]);
            if (knots.size() >= 10)
                visited9.insert(knots[9]);
        }
    }
};

void printVisited(const auto &visited) {
    Vec2l minPos{0, 0};
    Vec2l maxPos{0, 0};

    for (const auto &pos : visited) {
        minPos = {std::min(minPos.x, pos.x), std::min(minPos.y, pos.y)};
        maxPos = {std::max(maxPos.x, pos.x), std::max(maxPos.y, pos.y)};
    }

    for (const auto y : iota(minPos.y - 1, maxPos.y + 2)) {
        for (const auto x : iota(minPos.x - 1, maxPos.x + 2)) {
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
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0]
                  << " <input.txt> [--view=part1|part2]\n";
        std::exit(EXIT_FAILURE);
    }

    const std::string showPart = (argc == 3) ? argv[2] : "";

    Rope rope{};

    SimpleParser scanner{argv[1]};
    while (!scanner.isEof()) {
        const auto direction = scanner.getToken();
        const auto steps = scanner.getInt64();
        rope.move(direction, steps);
    }
    fmt::print("The second knot visited {} locations\n", visited1.size());
    if (showPart == "--view=part1") {
        printVisited(visited1);
    }
    fmt::print("The 10th knot visited {} locations\n", visited9.size());
    if (showPart == "--view=part2") {
        printVisited(visited9);
    }
    fmt::print("With at least {} knots the last one would not be moving\n",
               rope.knots.size());
}
