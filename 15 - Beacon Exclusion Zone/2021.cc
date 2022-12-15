#include <algorithm>
#include <fmt/format.h>
#include <fstream>
#include <iostream>
#include <ranges>
#include <string>
#include <unordered_set>
#include <vector>

#include "simpleparser.hpp"
#include "vec2.hpp"

using std::views::iota;

int64_t manhattan(const Vec2l &from, const Vec2l &to) {
    return std::abs(from.x - to.x) + std::abs(from.y - to.y);
}

struct Sensor {
    Vec2l position;
    Vec2l beacon;

    std::pair<int64_t, int64_t> scanInRow(int64_t row) const {
        const auto beaconDist = manhattan(position, beacon);
        const auto rowDist = std::abs(position.y - row);
        if (rowDist > beaconDist) {
            return {0, -1};
        }
        return {position.x - beaconDist + rowDist,
                position.x + beaconDist - rowDist};
    }
};

std::vector<Sensor> deployed{};

int64_t scanRow(int64_t row) {
    std::unordered_set<int64_t> scanned{};
    for (const auto &sensor : deployed) {
        const auto [from, to] = sensor.scanInRow(row);
        if (from <= to) {
            for (const auto posX : iota(from, to + 1)) {
                scanned.insert(posX);
            }
        }
    }
    for (const auto &sensor : deployed) {
        if (sensor.beacon.y == row) {
            scanned.erase(sensor.beacon.x);
        }
    }
    return scanned.size();
}

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input.txt>\n";
        std::exit(EXIT_FAILURE);
    }

    SimpleParser scanner{argv[1]};
    while (!scanner.isEof()) {
        scanner.skipToken("Sensor at x=");
        const auto sx = scanner.getInt64();
        scanner.skipToken(", y=");
        const auto sy = scanner.getInt64();
        scanner.skipToken(": closest beacon is at x=");
        const auto bx = scanner.getInt64();
        scanner.skipToken(", y=");
        const auto by = scanner.getInt64();
        deployed.emplace_back(Vec2l{sx, sy}, Vec2l{bx, by});
    }
    // const int64_t row = 10;
    const int64_t row = 2000000;
    fmt::print("There are {} spots on row {}\n", scanRow(row), row);
}
