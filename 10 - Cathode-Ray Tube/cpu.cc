#include <algorithm>
#include <cstdlib>
#include <fmt/format.h>
#include <fstream>
#include <iostream>
#include <ranges>
#include <string>
#include <vector>

#include "simpleparser.hpp"

using std::views::iota;

struct ALU {
    SimpleParser commands;

    int64_t cycle = 0;
    int64_t x = 1;
    enum num { empty, noop, addx } state = empty;
    int64_t busy = 0;
    int64_t parm = 0;

    int64_t signal = 0;

    ALU(char *filename) : commands(filename) {}

    void fetchOp() {
        if (commands.isEof()) {
            fmt::print("The sum of all signals is {}\n", signal);
            std::exit(EXIT_SUCCESS);
        }
        std::string opcode = commands.getToken();
        if (opcode == "addx") {
            state = addx;
            busy = 2;
            parm = commands.getInt64();
            // fmt::print("{:3d}: {} {}\n", cycle + 1, opcode, parm);
        } else if (opcode == "noop") {
            state = noop;
            busy = 1;
            // fmt::print("{:3d}: {}\n", cycle + 1, opcode);
        } else {
            fmt::print("Invalid opcode {} error\n", opcode);
            std::exit(EXIT_FAILURE);
        }
    }

    void paint() {
        if (std::abs(x - (cycle - 1) % 40) < 2) {
            std::cout << "☃";
        } else {
            std::cout << ' ';
        }
        if (cycle % 40 == 0) {
            std::cout << '\n';
        }
    }

    void step() {
        if (state == empty) {
            fetchOp();
        }
        ++cycle;
        if ((cycle == 20 or (cycle % 40) == 20) and cycle <= 220) {
            signal += x * cycle;
            // fmt::print("Step {} has signal {}\n", cycle, x * cycle);
        }
        paint();
        --busy;
        if (busy == 0) {
            if (state == addx) {
                x += parm;
            }
            state = empty;
        }
    }
    void loop() {
        while (true) {
            step();
        }
    }
};

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input.txt>\n";
        std::exit(EXIT_FAILURE);
    }

    ALU alu{argv[1]};
    alu.loop();
}
