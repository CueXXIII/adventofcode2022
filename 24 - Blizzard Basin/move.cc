#include <algorithm>
#include <array>
#include <fmt/format.h>
#include <fstream>
#include <iostream>
#include <numeric>
#include <queue>
#include <ranges>
#include <string>
#include <unordered_set>
#include <vector>

#include "utility.hpp"
#include "vec2.hpp"

using std::views::iota;

constexpr std::array<Vec2l, 5> direction{
    {{0, 0}, {0, -1}, {0, 1}, {-1, 0}, {1, 0}}};

struct Plateau {
    int64_t width{};
    int64_t height{};
    // can I move back to my start pos?
    // yes, but that would be the same as just waiting there.
    Vec2l startPos{0, -1};
    Vec2l goalPos{};

    std::vector<bool> blizzUp{};
    std::vector<bool> blizzDown{};
    std::vector<bool> blizzLeft{};
    std::vector<bool> blizzRight{};

    // path finding
    int64_t blizzardCycle{0};
    std::vector<bool> visited{};

    int64_t pos2ind(const Vec2l &pos) const { return pos.y * width + pos.x; }

    void resize() {
        blizzUp.resize(width * height, false);
        blizzDown.resize(width * height, false);
        blizzLeft.resize(width * height, false);
        blizzRight.resize(width * height, false);
        blizzardCycle = std::lcm(width, height);
    }

    bool hasBDown(const Vec2l &pos, const int64_t minute) const {
        return blizzDown[pos2ind(
            {pos.x, (pos.y + (height - 1) * minute) % height})];
    }
    bool hasBUp(const Vec2l &pos, const int64_t minute) const {
        return blizzUp[pos2ind({pos.x, (pos.y + minute) % height})];
    }
    bool hasBRight(const Vec2l &pos, const int64_t minute) const {
        return blizzRight[pos2ind(
            {(pos.x + (width - 1) * minute) % width, pos.y})];
    }
    bool hasBLeft(const Vec2l &pos, const int64_t minute) const {
        return blizzLeft[pos2ind({(pos.x + minute) % width, pos.y})];
    }
    bool hasBlizzard(const Vec2l &pos, const int64_t minute) const {
        return hasBUp(pos, minute) or hasBDown(pos, minute) or
               hasBLeft(pos, minute) or hasBRight(pos, minute);
    }

    void setBlizzard(const Vec2l &pos, char what) {
        switch (what) {
        case '^':
            blizzUp[pos2ind(pos)] = true;
            break;
        case 'v':
            blizzDown[pos2ind(pos)] = true;
            break;
        case '<':
            blizzLeft[pos2ind(pos)] = true;
            break;
        case '>':
            blizzRight[pos2ind(pos)] = true;
            break;
        }
    }

    void print(const Vec2l &playerPos, int64_t minute) {
        fmt::print("Minute {}\n", minute);
        for (const auto x : iota(-1, width + 1)) {
            if (playerPos == Vec2l{x, -1}) {
                std::cout << 'E';
                continue;
            }
            if (startPos.x != x) {
                std::cout << '#';
            } else {
                std::cout << '.';
            }
        }
        std::cout << '\n';
        for (const auto y : iota(0, height)) {
            std::cout << '#';
            for (const auto x : iota(0, width)) {
                const Vec2l pos{x, y};
                char out = '.';
                int count = 0;
                if (hasBUp(pos, minute)) {
                    out = '^';
                    ++count;
                }
                if (hasBDown(pos, minute)) {
                    out = 'v';
                    ++count;
                }
                if (hasBLeft(pos, minute)) {
                    out = '<';
                    ++count;
                }
                if (hasBRight(pos, minute)) {
                    out = '>';
                    ++count;
                }
                if (count > 1) {
                    out = static_cast<char>('0' + count);
                }
                if (playerPos == pos) {
                    // X = dead by blizzard player
                    out = (out == '.') ? 'E' : 'X';
                }
                std::cout << out;
            }
            std::cout << "#\n";
        }
        for (const auto x : iota(-1, width + 1)) {
            if (playerPos == Vec2l{x, height}) {
                std::cout << 'E';
                continue;
            }
            if (goalPos.x != x) {
                std::cout << '#';
            } else {
                std::cout << '.';
            }
        }
        std::cout << "\n\n";
    }

    // see if dest is free and reserve it. returns free state
    bool registerMove(const Vec2l &pos, const int64_t min) {
        if (pos == startPos or pos == goalPos) {
            return true;
        }
        if (pos.x < 0 or pos.y < 0 or pos.x >= width or pos.y >= height) {
            return false;
        }
        if (hasBlizzard(pos, min)) {
            return false;
        }
        const int64_t visPos =
            pos2ind(pos) + (min % blizzardCycle) * width * height;
        bool result = !visited[visPos];
        visited[visPos] = true;
        return result;
    }

    // A*
    // move out, but not where we came in
    int64_t findPath(const Vec2l &initialPos, int64_t startTime) {
        // int64_t singleStepCount{0}; // for debugging
        struct State {
            Vec2l pos;
            int64_t min;
        };
        auto compareState = [&](const State &a, const State &b) -> bool {
            const auto aScore = a.min + manhattan(a.pos, goalPos);
            const auto bScore = b.min + manhattan(b.pos, goalPos);
            return aScore > bScore;
        };
        std::priority_queue<State, std::vector<State>, decltype(compareState)>
            frontier{compareState};
        frontier.emplace(initialPos, startTime);
        visited.clear();
        visited.resize(width * height * blizzardCycle, false);
        while (!frontier.empty()) {
            const auto current = frontier.top();
            frontier.pop();
            /* // debug
            ++singleStepCount;
            if((singleStepCount % 1) == 0) {
                fmt::print("A*: step {} queuesize {}\n", singleStepCount,
            frontier.size()); print(current.pos, current.min);
            } */
            if (current.pos == initialPos) {
                // wait, but not forever
                if (current.min + 1 - startTime < blizzardCycle) {
                    frontier.emplace(current.pos, current.min + 1);
                }
            } else if (current.pos == startPos or current.pos == goalPos) {
                return current.min;
            }
            for (const auto &dir : direction) {
                if (current.pos + dir == initialPos) {
                    // don't move back to the start, we can wait there
                    continue;
                }
                if (registerMove(current.pos + dir, current.min + 1)) {
                    frontier.emplace(current.pos + dir, current.min + 1);
                }
            }
        }
        return -1; // not reached if path
    }
};

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input.txt>\n";
        std::exit(EXIT_FAILURE);
    }

    Plateau plat{};
    std::vector<std::string> winds{};

    std::ifstream infile{argv[1]};
    std::string line;
    std::getline(infile, line);
    plat.width = line.size() - 2;
    plat.startPos = {static_cast<int64_t>(line.find('.')) - 1, -1};
    while (std::getline(infile, line)) {
        if (line[1] == '#' or line[2] == '#') {
            break;
        }
        winds.push_back(line);
    }
    plat.height = winds.size();
    plat.goalPos = {static_cast<int64_t>(line.find('.')) - 1, plat.height};
    plat.resize();
    for (const auto y : iota(0, plat.height)) {
        for (const auto x : iota(0, plat.width)) {
            plat.setBlizzard({x, y}, winds[y][x + 1]);
        }
    }

    const auto way1 = plat.findPath(plat.startPos, 0);
    fmt::print("Reached the exit after {} minutes\n", way1);
    fmt::print("But we need to go back and get the snacks!\n");
    const auto way2 = plat.findPath(plat.goalPos, way1);
    fmt::print("The snacks are still cold (minute {})\n", way2);
    const auto way3 = plat.findPath(plat.startPos, way2);
    fmt::print("Now, that took {} minutes in the end\n", way3);
}
