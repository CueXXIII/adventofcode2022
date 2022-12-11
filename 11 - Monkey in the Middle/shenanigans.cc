#include <algorithm>
#include <fmt/format.h>
#include <fstream>
#include <iostream>
#include <ranges>
#include <string>
#include <vector>

#include "simpleparser.hpp"

using std::views::iota;

struct Monkey {
    std::vector<Monkey> &myFlock;

    int64_t id{};
    std::vector<int64_t> items{};
    enum { add, mul, square } op{};
    int64_t opArg{};
    int64_t divTest{};
    int64_t trueMonkey{};
    int64_t falseMonkey{};

    int64_t inspections{};

    Monkey(SimpleParser &input, std::vector<Monkey> &flock) : myFlock(flock) {
        // Monkey 0:
        input.skipToken("Monkey");
        id = input.getInt64();
        input.skipChar(':');
        // Starting items: 79, 98
        input.skipToken("Starting items:");
        do {
            items.push_back(input.getInt64());
        } while (input.skipChar(','));
        // Operation: new = old * 19
        input.skipToken("Operation: new = old");
        const auto t1 = input.getToken();
        const auto t2 = input.getToken();
        if (t2 == "old") {
            op = square;
        } else if (t1 == "*") {
            op = mul;
            opArg = std::stol(t2);
        } else if (t1 == "+") {
            op = add;
            opArg = std::stol(t2);
        } else {
            fmt::print("Parse error Operation old {} {}\n", t1, t2);
        }
        // Test: divisible by 23
        input.skipToken("Test: divisible by");
        divTest = input.getInt64();
        // If true: throw to monkey 2
        input.skipToken("If true: throw to monkey");
        trueMonkey = input.getInt64();
        // If false: throw to monkey 3
        input.skipToken("If false: throw to monkey");
        falseMonkey = input.getInt64();
    }

    void printItemList() const {
        if (items.size() > 0) {
            fmt::print("{}", items[0]);
            for (const auto item : items | std::views::drop(1)) {
                fmt::print(", {}", item);
            }
        }
        fmt::print("\n");
    }

    void print() const {
        fmt::print("Monkey {}:\n", id);
        fmt::print("  Starting items: ");
        printItemList();
        if (op == square) {
            fmt::print("  Operation: new = old * old\n");
        } else {
            fmt::print("  Operation: new = old {} {}\n",
                       std::array<const char *, 2>{"+", "*"}[op], opArg);
        }
        fmt::print("  Test: divisible by {}\n", divTest);
        fmt::print("    If true: throw to monkey {}\n", trueMonkey);
        fmt::print("    If false: throw to monkey {}\n\n", falseMonkey);
    }

    void printItems() const {
        fmt::print("Monkey {}: ", id);
        printItemList();
    }

    void changeWorry(int64_t &item) {
        switch (op) {
        case add:
            item += opArg;
            break;
        case mul:
            item *= opArg;
            break;
        case square:
            item *= item;
            break;
        }
    }

    void turn() {
        for (auto item : items) {
            ++inspections;
            // fmt::print("  Monkey inspects an item with a worry level of
            // {}.\n", item);
            changeWorry(item);
            // fmt::print("  Worry level is changed to {}.\n", item);
            item /= 3;
            // fmt::print("  Monkey gets bored with item. Worry level is divided
            // by 3 to {}\n", item);
            if (item % divTest == 0) {
                myFlock[trueMonkey].items.push_back(item);
            } else {
                myFlock[falseMonkey].items.push_back(item);
            }
        }
        items.clear();
    }
};

void round(std::vector<Monkey> &flock) {
    for (auto &monkey : flock) {
        monkey.turn();
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input.txt>\n";
        std::exit(EXIT_FAILURE);
    }

    std::vector<Monkey> flock;
    SimpleParser scanner{argv[1]};
    while (!scanner.isEof()) {
        flock.emplace_back(scanner, flock);
        // flock.back().print();
    }
    for ([[maybe_unused]] const auto _ : iota(0, 20)) {
        round(flock);
    }
    std::vector<int64_t> inspections{};
    for (const auto &monkey : flock) {
        fmt::print("Monkey {} inspected items {} times.\n", monkey.id,
                   monkey.inspections);
        inspections.push_back(monkey.inspections);
    }
    std::nth_element(inspections.begin(), inspections.begin() + 1,
                     inspections.end(), std::greater{});
    fmt::print("The first answer is {}\n", inspections[0] * inspections[1]);
}
