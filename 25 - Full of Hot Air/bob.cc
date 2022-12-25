#include <algorithm>
#include <array>
#include <fmt/format.h>
#include <fstream>
#include <iostream>
#include <ranges>
#include <string>
#include <vector>

using std::views::iota;

inline int64_t b5tob10(const char digit) {
    switch (digit) {
    case '0':
    case '1':
    case '2':
        return digit - '0';
    case '-':
        return -1;
    case '=':
        return -2;
    }
    return -666;
}

inline char b10to5(const int64_t digit) {
    const static std::array<char, 5> d{{'=', '-', '0', '1', '2'}};
    return d[digit + 2];
}

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input.txt>\n";
        std::exit(EXIT_FAILURE);
    }

    std::ifstream infile{argv[1]};
    std::string line;
    int64_t sum = 0;
    while (std::getline(infile, line)) {
        int64_t base = 1;
        int64_t number = 0;
        for (auto digit = line.rbegin(); digit != line.rend(); ++digit) {
            const auto dig = b5tob10(*digit);
            number += dig * base;
            base *= 5;
            if (base * 5 < base)
                fmt::print("possible overflow at {}\n", line);
        }
        fmt::print("{}₅ = {}₁₀\n", line, number);
        sum += number;
    }
    const auto result10 = sum;
    std::string result5{};
    while (sum != 0) {
        auto digit = sum % 5;
        if (digit > 2)
            digit -= 5;
        result5 = b10to5(digit) + result5;
        sum -= digit;
        sum /= 5;
    }
    fmt::print("The sum is {}₁₀, or {}₅\n", result10, result5);
}
