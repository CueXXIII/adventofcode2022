#include <algorithm>
#include <fmt/format.h>
#include <fstream>
#include <iostream>
#include <ranges>
#include <set>
#include <string>
#include <variant>
#include <vector>

#include "simpleparser.hpp"

using std::views::iota;

struct Value : public std::variant<int64_t, std::vector<Value>> {};

void printValue(const Value &val) {
    if (val.index() == 0) {
        fmt::print("{}", std::get<0>(val));
    } else {
        const auto data = std::get<1>(val);
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

auto operator<=>(const Value &left, const Value &right) {
    if (left.index() == 0) {
        if (right.index() == 0) {
            return std::get<0>(left) <=> std::get<0>(right);
        } else {
            return Value{std::vector<Value>{left}} <=> right;
        }
    } else {
        if (right.index() == 0) {
            return left <=> Value{std::vector<Value>{right}};
        } else {
            const auto &lv = std::get<1>(left);
            const auto &rv = std::get<1>(right);
            for (const auto pos : iota(0u, lv.size())) {
                if (pos >= rv.size()) {
                    return std::strong_ordering::greater;
                }
                if (lv[pos] < rv[pos]) {
                    return std::strong_ordering::less;
                }
                if (rv[pos] < lv[pos]) {
                    return std::strong_ordering::greater;
                }
            }
            if (rv.size() == lv.size()) {
                return std::strong_ordering::equal;
            }
            return std::strong_ordering::less;
        }
    }
}

std::vector<Value> readList(SimpleParser &scanner) {
    std::vector<Value> result{};
    while (!scanner.skipChar(']')) {
        if (scanner.skipChar('[')) {
            result.push_back({readList(scanner)});
        } else {
            result.push_back({scanner.getInt64()});
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

    std::set<Value> orderedData{};
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
        orderedData.insert(left);
        orderedData.insert(right);
    }
    fmt::print("The sum of the ordered indexes is {}\n", indexSum);

    Value d0{std::vector<Value>{{std::vector<Value>{{2}}}}};
    Value d1{std::vector<Value>{{std::vector<Value>{{6}}}}};
    orderedData.insert(d0);
    orderedData.insert(d1);
    int64_t index = 1;
    int64_t indexProd = 1;
    for (const auto &value : orderedData) {
        if (value == d0 or value == d1) {
            indexProd *= index;
        }
        ++index;
    }
    fmt::print("The product of the divider indices is {}\n", indexProd);
}
