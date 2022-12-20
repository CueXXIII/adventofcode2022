#include <algorithm>
#include <fmt/format.h>
#include <fstream>
#include <iostream>
#include <list>
#include <ranges>
#include <string>
#include <vector>

#include "simpleparser.hpp"

using std::views::iota;

const auto DECRYPTION_KEY{811589153};

std::vector<int64_t> data{};
std::vector<int64_t> position{};

int64_t dataAt(const int64_t pos) {
    for (const auto i : iota(0, (int64_t)position.size())) {
        if (position[i] == pos % (int64_t)position.size())
            return data[i];
    }
    fmt::print("dataAt({})\n", pos);
    throw("not reached");
}

int64_t pMod(int64_t n, int64_t m) { return (n % m + m) % m; }

void printData() {
    fmt::print("{}", dataAt(0));
    for (const auto pos : iota(1, (int64_t)position.size())) {
        fmt::print(", {}", dataAt(pos));
    }
    // fmt::print("  [{}",position[0]);
    // for(const auto &pos : position | std::views::drop(1)) {
    //     fmt::print(", {}", pos);
    // }
    fmt::print("\n");
}

void mix() {
    const auto modulus = (int64_t)data.size();
    for (const auto i : iota(0, modulus)) {
        if (data[i] % (modulus - 1) == 0)
            continue;
        const auto oldPos = position[i];
        const auto newRawPos = oldPos + data[i] % (modulus - 1);
        const auto newPos = [&]() {
            if (0 < newRawPos and newRawPos < modulus)
                return newRawPos;
            if (newRawPos <= 0)
                return newRawPos + modulus - 1;
            return newRawPos - modulus + 1;
        }();
        for (auto &ele : position) {
            if (oldPos < newPos and oldPos <= ele and ele <= newPos) {
                ele = (ele - 1 + modulus) % modulus;
            } else if (newPos < oldPos and newPos <= ele and ele <= oldPos) {
                ele = (ele + 1) % modulus;
            }
        }
        position[i] = newPos;
        // printData();
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input.txt>\n";
        std::exit(EXIT_FAILURE);
    }

    SimpleParser scanner{argv[1]};
    int64_t index = 0;
    int64_t zeroPos = 0;
    while (!scanner.isEof()) {
        data.push_back(scanner.getInt64() * DECRYPTION_KEY);
        if (data.back() == 0) {
            zeroPos = index;
        }
        position.push_back(index);
        ++index;
    }
    // printData();
    for ([[maybe_unused]] const auto count : iota(0, 10)) {
        mix();
    }
    // printData();
    const auto d1000 = dataAt(1000 + position[zeroPos]);
    const auto d2000 = dataAt(2000 + position[zeroPos]);
    const auto d3000 = dataAt(3000 + position[zeroPos]);
    fmt::print("{}+{}+{}={}\n", d1000, d2000, d3000, d1000 + d2000 + d3000);
}
