#include <fmt/format.h>
#include <fstream>
#include <string>

#include "simpleparser.hpp"

int main(int, char **argv) {
    std::ifstream infile{argv[1]};
    SimpleParser assigns{infile};
    int64_t overlapCounter = 0;
    while (!assigns.isEof()) {
        const auto range1s = assigns.getInt64();
        assigns.skipChar('-');
        const auto range1e = assigns.getInt64();
        assigns.skipChar(',');
        const auto range2s = assigns.getInt64();
        assigns.skipChar('-');
        const auto range2e = assigns.getInt64();

        if (range1s <= range2s and range1e >= range2e) {
            fmt::print("{}-{} overlaps {}-{}\n", range2s, range2e, range1s,
                       range1e);
            ++overlapCounter;
        } else if (range2s <= range1s and range2e >= range1e) {
            fmt::print("{}-{} overlaps {}-{}\n", range1s, range1e, range2s,
                       range2e);
            ++overlapCounter;
        }
    }
    fmt::print("I have detected {} overlaps\n", overlapCounter);
}
