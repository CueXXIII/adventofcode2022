#include <fmt/format.h>
#include <fstream>
#include <string>

int main(int, char **argv) {
    std::ifstream infile{argv[1]};
    int64_t maxCalories = 0;
    std::string line{};
    while (!infile.eof()) {
        int64_t currentCalories = 0;
        while (std::getline(infile, line)) {
            if (line.size() == 0) {
                break;
            }
            currentCalories += stol(line);
        }
        fmt::print("{}\n", currentCalories);
        if (currentCalories > maxCalories) {
            maxCalories = currentCalories;
        }
    }
    fmt::print("\n");
    fmt::print("Any elf carries at most {} calories.\n", maxCalories);
}
