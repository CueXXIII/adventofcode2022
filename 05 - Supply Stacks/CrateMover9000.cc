#include <fmt/format.h>
#include <fstream>
#include <ranges>
#include <stack>
#include <string>
#include <type_traits>
#include <vector>

#include "simpleparser.hpp"

using std::views::iota;

std::vector<std::stack<char>> cargoHold{};

template <typename T> void reverse(T &stack) {
    T reverse{};
    while (!stack.empty()) {
        reverse.push(stack.top());
        stack.pop();
    }
    stack = reverse;
}

int main(int, char **argv) {
    std::ifstream infile{argv[1]};
    std::string line;

    std::getline(infile, line);
    const auto numCrates = (line.size() + 1) / 4;
    cargoHold.resize(numCrates);

    do {
        for (const auto i : iota(0u, numCrates)) {
            if (line[i * 4] == '[') {
                cargoHold[i].push(line[i * 4 + 1]);
            }
        }
        std::getline(infile, line);
    } while (line.size() > 0);
    for (auto &stack : cargoHold) {
        reverse(stack);
    }

    SimpleParser moves{infile};
    while (!moves.isEof()) {
        moves.skipToken("move");
        const auto num = moves.getInt64();
        moves.skipToken("from");
        const auto src = moves.getInt64() - 1;
        moves.skipToken("to");
        const auto dst = moves.getInt64() - 1;

        for ([[maybe_unused]] const auto i : iota(0, num)) {
            cargoHold[dst].push(cargoHold[src].top());
            cargoHold[src].pop();
        }
    }

    for (const auto &stack : cargoHold) {
        fmt::print("{}", stack.top());
    }
    fmt::print("\n");
}
