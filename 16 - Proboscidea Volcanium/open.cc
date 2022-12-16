#include <algorithm>
#include <fmt/format.h>
#include <fstream>
#include <iostream>
#include <map>
#include <ranges>
#include <set>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "simpleparser.hpp"

using std::views::iota;

struct Room {
    std::string name;
    std::vector<std::string> tunnels;
    int64_t pressure;

    // highest pressure first
    auto operator<=>(const Room &other) {
        if (other.pressure == pressure) {
            return name <=> other.name;
        }
        return other.pressure <=> pressure;
    }
};

std::unordered_map<std::string, Room> cave{};
std::map<std::string, Room> pressurized{};

// initialized by fillFloydWarshall()
std::map<std::pair<std::string, std::string>, int64_t> shortestPath{};

void fillFloydWarshall() {
    // https://en.wikipedia.org/wiki/Floyd%E2%80%93Warshall_algorithm
    for (const auto &[u, room] : cave) {
        for (const auto &v : room.tunnels) {
            shortestPath[{u, v}] = 1;
        }
        shortestPath[{u, u}] = 0;
    }
    for (const auto &[k, _1] : cave) {
        for (const auto &[i, _2] : cave) {
            if (!shortestPath.contains({i, k})) {
                continue;
            }
            for (const auto &[j, _3] : cave) {
                if (!shortestPath.contains({k, j})) {
                    continue;
                }

                if (shortestPath.contains({i, j})) {
                    shortestPath[{i, j}] =
                        std::min(shortestPath[{i, j}],
                                 shortestPath[{i, k}] + shortestPath[{k, j}]);
                } else {
                    shortestPath[{i, j}] =
                        shortestPath[{i, k}] + shortestPath[{k, j}];
                }
            }
        }
    }
    fmt::print("Populated {} paths\n", shortestPath.size());
}

std::unordered_map<std::string, int64_t> memo{};

int64_t findPathWithElephant(const int64_t minutes, const std::string &meRoom,
                             const std::string &eleRoom,
                             const int64_t meTime = 0,
                             const int64_t eleTime = 0,
                             std::set<std::string> valvesOpened = {}) {
    if (valvesOpened.size() == pressurized.size()) {
        return 0;
    }
    std::stringstream memoKey;
    memoKey << fmt::format("{}@{},{}@{},", meRoom, meTime, eleRoom, eleTime);
    for (const auto &valve : valvesOpened) {
        memoKey << valve;
    }
    if (memo.contains(memoKey.str())) {
        return memo[memoKey.str()];
    }
    // fmt::print("memoKey = {}\n",memoKey.str());
    int64_t pressure = 0;
    for (const auto &[destName, destRoom] : pressurized) {
        if (valvesOpened.contains(destName)) {
            continue;
        }
        const int64_t meReleaseTime =
            meTime + shortestPath[{meRoom, destName}] + 1;
        const int64_t eleReleaseTime =
            eleTime + shortestPath[{eleRoom, destName}] + 1;
        if (meReleaseTime < eleReleaseTime) {
            // try me
            if (meReleaseTime <= minutes) {
                const int64_t releasePressure =
                    destRoom.pressure * (minutes - meReleaseTime);
                valvesOpened.insert(destName);
                const auto mePressure =
                    findPathWithElephant(minutes, destName, eleRoom,
                                         meReleaseTime, eleTime, valvesOpened);
                valvesOpened.erase(destName);

                pressure = std::max(pressure, releasePressure + mePressure);
            }
        } else {
            // try my elephant
            if (eleReleaseTime <= minutes) {
                const int64_t releasePressure =
                    destRoom.pressure * (minutes - eleReleaseTime);
                valvesOpened.insert(destName);
                const auto elePressure =
                    findPathWithElephant(minutes, meRoom, destName, meTime,
                                         eleReleaseTime, valvesOpened);
                valvesOpened.erase(destName);

                pressure = std::max(pressure, releasePressure + elePressure);
            }
        }
    }

    memo[memoKey.str()] = pressure;
    return pressure;
}

int64_t findPath(const int64_t minutes, const std::string &roomName,
                 const int64_t time = 0,
                 std::set<std::string> valvesOpened = {}) {
    if (valvesOpened.size() == pressurized.size()) {
        return 0;
    }
    int64_t pressure = 0;
    for (const auto &[destName, destRoom] : pressurized) {
        if (valvesOpened.contains(destName)) {
            continue;
        }
        const int64_t releaseTime =
            time + shortestPath[{roomName, destName}] + 1;
        if (releaseTime <= minutes) {
            const int64_t releasePressure =
                destRoom.pressure * (minutes - releaseTime);
            valvesOpened.insert(destName);
            const auto subPressure =
                findPath(minutes, destName, releaseTime, valvesOpened);
            valvesOpened.erase(destName);

            pressure = std::max(pressure, releasePressure + subPressure);
        }
    }

    return pressure;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input.txt>\n";
        std::exit(EXIT_FAILURE);
    }

    SimpleParser scanner{argv[1]};
    while (!scanner.isEof()) {
        scanner.skipToken("Valve");
        const auto name = scanner.getToken();
        scanner.skipToken("has flow rate=");
        const auto pressure = scanner.getInt64();
        if (!scanner.skipToken("; tunnel leads to valve")) {
            scanner.skipToken("; tunnels lead to valves");
        }
        decltype(Room::tunnels) dst{};
        do {
            dst.push_back(scanner.getToken(','));
        } while (scanner.skipChar(','));
        cave.emplace(name, Room{name, dst, pressure});
        if (pressure > 0) {
            pressurized.emplace(name, cave[name]);
        }
    }
    // std::sort(pressurized.begin(), pressurized.end());
    fillFloydWarshall();

    const auto releasedPressure = findPath(30, "AA");
    fmt::print("You can release a pressure of {}\n", releasedPressure);

    const auto releasedTwo = findPathWithElephant(26, "AA", "AA");
    fmt::print("You and your elephant can release a pressure of {}\n",
               releasedTwo);
}
