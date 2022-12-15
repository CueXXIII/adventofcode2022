#include <algorithm>
#include <fmt/format.h>
#include <fstream>
#include <iostream>
#include <list>
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
    Vec2l position{};
    Vec2l beacon{};
    int64_t beaconDist{};

    Sensor(const Vec2l &position, const Vec2l &beacon)
        : position(position), beacon(beacon),
          beaconDist(manhattan(position, beacon)) {}

    std::pair<int64_t, int64_t> scanInRow(int64_t row) const {
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

struct Intervals {
    std::list<std::pair<int64_t, int64_t>> intervals;

    void insert(int64_t left, int64_t right) {
        // fmt::print("insert [{}-{}] \n  into  ", left, right);
        // print();
        if (intervals.empty()) {
            intervals.emplace_back(left, right);
            // fmt::print("\n  gives "); print(); fmt::print("\n\n");
            return;
        }
        decltype(intervals) newIntervals{};
        while (!intervals.empty()) {
            if (right + 1 < intervals.front().first) {
                newIntervals.emplace_back(left, right);
                intervals.insert(intervals.begin(), newIntervals.begin(),
                                 newIntervals.end());
                // fmt::print("\n  gives "); print(); fmt::print("\n\n");
                return;
            }
            if (left - 1 > intervals.front().second) {
                newIntervals.push_back(intervals.front());
                intervals.pop_front();
                if (intervals.empty()) {
                    newIntervals.emplace_back(left, right);
                }
                continue;
            }
            // store overlap in [left, right]
            left = std::min(left, intervals.front().first);
            right = std::max(right, intervals.front().second);
            intervals.pop_front();
            if (intervals.empty()) {
                newIntervals.emplace_back(left, right);
            }
        }
        intervals = std::move(newIntervals);
        // fmt::print("\n  gives "); print(); fmt::print("\n\n");
    }

    int64_t size() const {
        print();
        fmt::print("\n");
        return intervals.size() == 1 ? 0 : 1;
    }

    int64_t spot() const {
        if (intervals.size() > 1) {
            return intervals.front().second + 1;
        }
        if (intervals.front().first == 1) {
            return 0;
        }
        return intervals.back().second + 1;
    }

    void print() const {
        for (const auto &[left, right] : intervals) {
            fmt::print("[{}-{}] ", left, right);
        }
    }
};

// const int64_t maxScan = 20;
const int64_t maxScan = 4000000;

int64_t scanEmptyRow(int64_t row) {
    Intervals scanned;
    for (const auto &sensor : deployed) {
        const auto [from, to] = sensor.scanInRow(row);
        if (from <= to and from <= maxScan and to >= 0) {
            scanned.insert(std::max(from, 0l), std::min(to, maxScan));
        }
    }
    return scanned.spot();
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

    for (const auto row2 : iota(0, maxScan + 1)) {
        const auto spot = scanEmptyRow(row2);
        if (spot != maxScan + 1) {
            fmt::print("There is a spot on row [{}, {}] = {}\n", spot, row2,
                       spot * 4000000 + row2);
        }
    }
}
