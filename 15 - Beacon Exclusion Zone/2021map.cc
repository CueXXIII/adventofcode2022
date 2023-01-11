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

int64_t probeRow = 2000000;
int64_t maxScan = 4000000;

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
    std::map<int64_t, int64_t> intervals;

    void insert(int64_t left, int64_t right) {
        // fmt::print("insert [{}-{}] \n  into  ", left, right);
        // print();

        // see if our start is already in the list
        auto next = intervals.lower_bound(left);
        if (next != intervals.end() and next->first == left) {
            next->second = std::max(next->second, right);
        } else {
            // try to keep the insert cheap
            next = intervals.emplace_hint(next, left, right);
        }
        // we want to start trying to merge this element with the previous
        // but if we are at the start, skip that step
        auto prev = next;
        if (next != intervals.begin()) {
            --prev;
        } else {
            ++next;
        }
        // can we merge prev and next?
        while (next != intervals.end()) {
            if (next->first <= prev->second + 1) {
                prev->second = std::max(prev->second, next->second);
                next = intervals.erase(next);
            } else if (prev->first < left) {
                ++prev;
                ++next;
            } else {
                break;
            }
        }
    }

    // find an empty spot for 2022d15
    int64_t spot() const {
        if (intervals.size() > 1) {
            return intervals.begin()->second + 1;
        }
        if (intervals.begin()->first == 1) {
            return 0;
        }
        return intervals.rbegin()->second + 1;
    }

    void print() const {
        for (const auto &[left, right] : intervals) {
            fmt::print("[{}-{}] ", left, right);
        }
    }
};

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

    if (argv[1][0] == 'e') {
        // we are giving example.txt or similar, change parameters
        probeRow = 10;
        maxScan = 20;
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
    fmt::print("There are {} spots on row {}\n", scanRow(probeRow), probeRow);

    for (const auto row2 : iota(0, maxScan + 1)) {
        const auto spot = scanEmptyRow(row2);
        if (spot != maxScan + 1) {
            fmt::print("There is a spot at [{}, {}] = {}\n", spot, row2,
                       spot * 4000000 + row2);
            break;
        }
    }
}
