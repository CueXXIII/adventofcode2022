#include <array>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

struct SNAFU {
    int64_t value;

    SNAFU(const std::string &from) {
        int64_t base = 1;
        value = 0;
        // digit: DIGit ITerator
        for (auto digit = from.rbegin(); digit != from.rend(); ++digit) {
            const auto dig = [](const auto cell) {
                switch (cell) {
                case '-':
                    return -1;
                case '=':
                    return -2;
                default:
                    return cell - '0';
                }
            }(*digit);
            value += dig * base;
            base *= 5;
            if (base * 5 < base)
                fmt::print("possible overflow at {}\n", from);
        }
    }
    SNAFU(const int64_t &from) : value(from) {}

    SNAFU &operator+=(const SNAFU &other) {
        value += other.value;
        return *this;
    }

    friend std::ostream &operator<<(std::ostream &os, const SNAFU &number) {
        const static std::array<char, 5> d{{'=', '-', '0', '1', '2'}};
        std::vector<char> result{};
        auto from = number.value;
        while (from != 0) {
            auto digit = from % 5;
            if (digit > 2)
                digit -= 5;
            result.push_back(d.at(static_cast<size_t>(digit + 2)));
            from -= digit;
            from /= 5;
        }
        for (auto resit = result.rbegin(); resit != result.rend(); ++resit) {
            os << *resit;
        }
        return os;
    }
};
template <> struct fmt::formatter<SNAFU> : ostream_formatter {};

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input.txt>\n";
        std::exit(EXIT_FAILURE);
    }

    std::ifstream infile{argv[1]};
    std::string line;
    SNAFU sum{0};
    while (std::getline(infile, line)) {
        const SNAFU number{line};
        fmt::print("{}₅ = {}₁₀\n", line, number.value);
        sum += number;
    }
    fmt::print("The sum is {}₁₀, or {}₅\n", sum.value, sum);
}
