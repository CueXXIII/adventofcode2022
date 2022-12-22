#include <algorithm>
#include <fmt/format.h>
#include <fstream>
#include <iostream>
#include <ranges>
#include <string>
#include <unordered_map>
#include <vector>

#include "simpleparser.hpp"
#include "vec2.hpp"

using std::views::iota;

// -1: turn left, +1: turn right
enum facing { right = 0, down = 1, left = 2, up = 3 };
static const std::vector<Vec2l> direction{{1, 0}, {0, 1}, {-1, 0}, {0, -1}};

// strange map, with border intervals in Vec2l
std::unordered_map<Vec2l, char> stMap{};

std::pair<Vec2l, int64_t> getNextPos(const Vec2l &myPos, const int64_t myFace) {
    const auto Q = 50; // cube size
    auto nextPos = myPos + direction[myFace];
    auto nextFace = myFace;
    // walk around the edge
    if (myFace == right) {
        if (nextPos.x == 3 * Q + 1) {
            // 5 ->  6
            nextPos.y = Q + 1 - (nextPos.y - 0 * Q) + 2 * Q;
            nextPos.x = 2 * Q;
            nextFace = left;
        } else if (nextPos.x == 2 * Q + 1 and nextPos.y >= Q + 1 and nextPos.y <= 2 * Q) {
            // 4 -> 5
            nextPos.x = nextPos.y + Q;
            nextPos.y = Q;
            nextFace = up;
        } else if (nextPos.x == 2 * Q + 1 and nextPos.y >= 2 * Q + 1) {
            // 6 -> 5
            nextPos.y = Q + 1 - (nextPos.y - 2 * Q) + 0 * Q;
            nextPos.x = 3 * Q;
            nextFace = left;
        } else if (nextPos.x == Q + 1 and nextPos.y >= 3 * Q + 1) {
            // 3 -> 6
            nextPos.x = nextPos.y - 2 * Q;
            nextPos.y = 3 * Q;
            nextFace = up;
        }
    } else if (myFace == down) {
        if (nextPos.y == 4 * Q + 1) {
            // 3 -> 5
            nextPos.x = nextPos.x + 2 * Q;
            nextPos.y = 1;
            nextFace = down;
        } else if (nextPos.y == 3 * Q + 1 and nextPos.x >= Q + 1) {
            // 6 -> 3
            nextPos.y = nextPos.x + 2 * Q;
            nextPos.x = Q;
            nextFace = left;
        } else if (nextPos.y == Q + 1 and nextPos.x >= 2 * Q + 1) {
            // 5 -> 4
            nextPos.y = nextPos.x - Q;
            nextPos.x = 2 * Q;
            nextFace = left;
        }
    } else if (myFace == left) {
        if (nextPos.x == Q and nextPos.y <= Q) {
            // 1 -> 2
            nextPos.y = Q + 1 - (nextPos.y - 0 * Q) + 2 * Q;
            nextPos.x = 1;
            nextFace = right;
        } else if (nextPos.x == Q and nextPos.y >= Q + 1 and nextPos.y <= 2 * Q) {
            // 4 -> 2
            nextPos.x = nextPos.y - Q;
            nextPos.y = 2 * Q + 1;
            nextFace = down;
        } else if (nextPos.x == 0 and nextPos.y <= 3 * Q) {
            // 2 -> 1
            nextPos.y = Q + 1 - (nextPos.y - 2 * Q) + 0 * Q;
            nextPos.x = Q + 1;
            nextFace = right;
        } else if (nextPos.x == 0 and nextPos.y >= 3 * Q + 1) {
            // 3 -> 1
            nextPos.x = nextPos.y - 2 * Q;
            nextPos.y = 1;
            nextFace = down;
        }
    } else if (myFace == up) {
        if (nextPos.y == 2 * Q and nextPos.x <= Q) {
            // 2 -> 4
            nextPos.y = nextPos.x + Q;
            nextPos.x = Q + 1;
            nextFace = right;
        } else if (nextPos.y == 0 and nextPos.x <= 2 * Q) {
            // 1 -> 3
            nextPos.y = nextPos.x + 2 * Q;
            nextPos.x = 1;
            nextFace = right;
        } else if (nextPos.y == 0 and nextPos.x >= 2 * Q + 1) {
            // 5 -> 3
            nextPos.x = nextPos.x - 2 * Q;
            nextPos.y = 4 * Q;
            nextFace = up;
        }
    }
    return {nextPos, nextFace};
}

// other cube, leave incomplete
std::pair<Vec2l, int64_t> getNextPosExample(const Vec2l &myPos, const int64_t myFace) {
    const auto Q = 4; // cube size
    auto nextPos = myPos + direction[myFace];
    auto nextFace = myFace;
    // walk around the edge
    // TODO check facing
    if (nextPos.y == 0) {
        // 1 -> 2
        nextPos.x = Q + 1 - (nextPos.x - 2 * Q) + 0 * Q;
        nextPos.y = Q + 1;
        nextFace = down;
    } else if (nextPos.x == 3 * Q + 1 and nextPos.y <= Q) {
        // 1 -> 6
        nextPos.y = Q + 1 - (nextPos.y - 0 * Q) + 2 * Q;
        nextPos.x = 3 * Q;
        nextFace = left;
    } else if (nextPos.x == 3 * Q + 1 and nextPos.y >= Q + 1 and nextPos.y <= 2 * Q) {
        // 4 -> 6
        nextPos.x = Q + 1 - (nextPos.y - 1 * Q) + 3 * Q;
        nextPos.y = 2 * Q + 1;
        nextFace = down;
    } // TODO else
    else if (nextPos.y == 3 * Q + 1 and nextPos.x <= 3 * Q) {
        // 5 -> 2
        nextPos.x = Q + 1 - (nextPos.x - 2 * Q) + 0 * Q;
        nextPos.y = 2 * Q;
        nextFace = up;
    } // TODO else
    else if (nextPos.y == Q and nextPos.x >= Q + 1 and nextPos.x <= 2 * Q) {
        // 3 -> 1
        nextPos.y = nextPos.x - Q;
        nextPos.x = 2 * Q + 1;
        nextFace = right;
    } // TODO else
    return {nextPos, nextFace};
}

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input.txt>\n";
        std::exit(EXIT_FAILURE);
    }
    const bool isExample = std::string{argv[1]} == "example.txt";

    std::ifstream infile{argv[1]};
    std::string line;

    // int64_t row=1;
    for (int64_t row = 1; std::getline(infile, line); ++row) {
        // while (std::getline(infile, line)) {
        if (line.size() == 0)
            break;
        for (const auto i : iota(0u, line.size())) {
            if (line[i] != ' ') {
                const int64_t col = i + 1;
                stMap[{col, row}] = line[i];
            }
        }
    }

    Vec2l myPos{1, 1};
    while ((!stMap.contains(myPos)) or stMap[myPos] == '#') {
        ++myPos.x;
    }
    int64_t myFace = right;

    SimpleParser scanner{infile};
    while (!scanner.isEof()) {
        const auto len = scanner.getInt64();
        // fmt::print("forward {}\n",len);
        for ([[maybe_unused]] const auto i : iota(0, len)) {
            const auto [nextPos, nextFace] =
                isExample ? getNextPosExample(myPos, myFace) : getNextPos(myPos, myFace);
            if (!stMap.contains(nextPos))
                fmt::print("error {}\n", nextPos);
            if (stMap[nextPos] == '#') {
                break;
            }
            myPos = nextPos;
            myFace = nextFace;
        }
        if (scanner.isEof()) {
            break;
        }
        // TODO scanner api
        const auto turn = scanner.peekChar();
        scanner.skipChar(turn);
        // fmt::print("turn {}\n",turn);
        switch (turn) {
        case 'L':
            myFace = (myFace + 3) % 4;
            break;
        case 'R':
            myFace = (myFace + 1) % 4;
            break;
        }
    }

    // fmt::print("({}, {}) face {}\n", myPos.x, myPos.y, myFace);
    fmt::print("The password is {}\n", 1000 * myPos.y + 4 * myPos.x + myFace);
}
