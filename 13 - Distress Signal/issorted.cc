#include <algorithm>
#include <fmt/format.h>
#include <fstream>
#include <iostream>
#include <ranges>
#include <string>
#include <variant>
#include <vector>

#include "simpleparser.hpp"

using std::views::iota;

struct Value {
    std::variant<int64_t, std::vector<Value>> data{};
    // Value(const int64_t val) : data(val) {}
    // Value(std::vector<Value> val) : data(val) {}
};

void printValue(const Value &val) {
    if (val.data.index() == 0) {
        fmt::print("{}", std::get<0>(val.data));
    } else {
        const auto data = std::get<1>(val.data);
        fmt::print("[");
        if (data.size() > 0) {
            printValue(data[0]);
            for (const auto &more : data | std::views::drop(1)) {
                fmt::print(",");
                printValue(more);
            }
        }
        fmt::print("]");
    }
}

bool operator<(const Value &left, const Value &right) {
    if (left.data.index() == 0) {
        if (right.data.index() == 0) {
            return std::get<0>(left.data) < std::get<0>(right.data);
        } else {
            return Value{std::vector<Value>{{std::get<0>(left.data)}}} < right;
        }
    } else {
        if (right.data.index() == 0) {
            return left < Value{std::vector<Value>{{std::get<0>(right.data)}}};
        } else {
            const auto &lv = std::get<1>(left.data);
            const auto &rv = std::get<1>(right.data);
            for (size_t pos = 0; pos < lv.size(); ++pos) {
                if (pos >= rv.size()) {
                    return false;
                }
                if (lv[pos] < rv[pos]) {
                    return true;
                }
                if (rv[pos] < lv[pos]) {
                    return false;
                }
            }
            if (rv.size() == lv.size()) {
                return false;
            }
            return true;
        }
    }
}

std::vector<Value> readList(SimpleParser &scanner) {
    std::vector<Value> result{};
    while (!scanner.skipChar(']')) {
        if (scanner.skipChar('[')) {
            result.emplace_back(readList(scanner));
        } else {
            result.emplace_back(scanner.getInt64());
        }
        scanner.skipChar(',');
    }
    return result;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input.txt>\n";
        std::exit(EXIT_FAILURE);
    }

    SimpleParser scanner{argv[1]};
    int64_t pair = 0;
    int64_t indexSum = 0;
    while (!scanner.isEof()) {
        scanner.skipChar('[');
        Value left{readList(scanner)};
        scanner.skipChar('[');
        Value right{readList(scanner)};
        bool rightOrder = left < right;
        fmt::print("Pair {} is {}in the right order\n", ++pair,
                   rightOrder ? "" : "not ");
        if (rightOrder) {
            indexSum += pair;
        }
    }
    fmt::print("The sum of the ordered indexes is {}\n", indexSum);
}
