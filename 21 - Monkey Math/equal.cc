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
    Monkey &rcalc();
};

std::map<std::string, Monkey> yells{};
std::map<std::string, Monkey> waits{};

Monkey &Monkey::rcalc() {
    if (yells.contains(op2)) {
        auto &mon1 = waits[op1];
        const auto &res2 = yells[op2].result;
        switch (op) {
        case '+':
            mon1.result = result - res2;
            return mon1;
        case '-':
            mon1.result = result + res2;
            return mon1;
        case '*':
            mon1.result = result / res2;
            return mon1;
        case '/':
            mon1.result = result * res2;
            return mon1;
        case '=':
            mon1.result = res2;
            return mon1;
        }
    } else {
        auto &mon2 = waits[op2];
        const auto &res1 = yells[op1].result;
        switch (op) {
        case '+':
            mon2.result = result - res1;
            return mon2;
        case '-':
            mon2.result = res1 - result;
            return mon2;
        case '*':
            mon2.result = result / res1;
            return mon2;
        case '/':
            mon2.result = res1 / result;
            return mon2;
        case '=':
            mon2.result = res1;
            return mon2;
        }
    }
    throw "not reached";
}

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
            if (name != "humn") {
                yells[name] = monkey;
            }
        } else {
            const auto op1 = scanner.getToken();
            const auto op = scanner.getToken()[0];
            const auto op2 = scanner.getToken();
            waits[name] = Monkey(name, op, op1, op2);
        }
    }
    waits["root"].op = '=';
    while (true) {
        bool yelling = false;
        for (auto &[name, monkey] : waits) {
            if (yells.contains(monkey.op1) and yells.contains(monkey.op2)) {
                monkey.calculate(yells[monkey.op1], yells[monkey.op2]);
                yells[name] = monkey;
                waits.erase(name);
                yelling = true;
                break;
            }
        }
        if (!yelling) {
            break;
        }
    }
    waits["humn"] = Monkey{"humn"};
    Monkey &trace = waits["root"];
    while (trace.name != "humn") {
        trace = trace.rcalc();
    }
    fmt::print("You yell: {}\n", waits["humn"].result);
}
