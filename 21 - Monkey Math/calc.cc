#include <algorithm>
#include <fmt/format.h>
#include <fstream>
#include <iostream>
#include <map>
#include <ranges>
#include <string>
#include <vector>

#include "simpleparser.hpp"

using std::views::iota;

struct Monkey {
    std::string name{};
    char op{};
    std::string op1{};
    std::string op2{};
    int64_t result{};

    void calculate(const Monkey &lhs, const Monkey &rhs) {
        const auto lval = lhs.result;
        const auto rval = rhs.result;
        switch (op) {
        case '+':
            result = lval + rval;
            break;
        case '-':
            result = lval - rval;
            break;
        case '*':
            result = lval * rval;
            break;
        case '/':
            result = lval / rval;
            break;
        }
    }
};

std::map<std::string, Monkey> yells{};
std::map<std::string, Monkey> waits{};

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input.txt>\n";
        std::exit(EXIT_FAILURE);
    }

    SimpleParser scanner{argv[1]};
    while (!scanner.isEof()) {
        const auto name = scanner.getToken(':');
        scanner.skipChar(':');
        if (std::isdigit(scanner.peekChar())) {
            Monkey monkey{name};
            monkey.result = scanner.getInt64();
            yells[name] = monkey;
        } else {
            const auto op1 = scanner.getToken();
            const auto op = scanner.getToken()[0];
            const auto op2 = scanner.getToken();
            waits[name] = Monkey(name, op, op1, op2);
        }
    }
    while (!yells.contains("root")) {

        for (auto &[name, monkey] : waits) {
            if (yells.contains(monkey.op1) and yells.contains(monkey.op2)) {
                monkey.calculate(yells[monkey.op1], yells[monkey.op2]);
                yells[name] = monkey;
                waits.erase(name);
                break;
            }
        }
    }
    fmt::print("You hear a yell: {}\n", yells["root"].result);
}
