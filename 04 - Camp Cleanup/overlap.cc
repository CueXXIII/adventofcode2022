#include <fmt/format.h>
#include <fstream>
#include <string>

#include "simpleparser.hpp"

int main(int, char **argv) {
    std::ifstream infile{argv[1]};
    SimpleParser assigns{infile};
    int64_t overlapCounter = 0;
    int64_t partialOverlaps = 0;
    while (!assigns.isEof()) {
        const auto range1s = assigns.getInt64();
        assigns.skipChar('-');
        const auto range1e = assigns.getInt64();
        assigns.skipChar(',');
        const auto range2s = assigns.getInt64();
        assigns.skipChar('-');
        const auto range2e = assigns.getInt64();

        // part 1
        if (range1s <= range2s and range1e >= range2e) {
            ++overlapCounter;
        } else if (range2s <= range1s and range2e >= range1e) {
            ++overlapCounter;
        }

        // part 2
        if (not(range1s > range2e or range2s > range1e)) {
            ++partialOverlaps;
        }
    }
    fmt::print("I have detected {} total overlaps\n", overlapCounter);
    fmt::print("And there are a total of {} partial overlaps\n",
               partialOverlaps);
}
