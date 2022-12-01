#include <algorithm>
#include <fmt/format.h>
#include <fstream>
#include <string>
#include <vector>

int main(int, char **argv) {
    std::ifstream infile{argv[1]};
    std::vector<int64_t> calories{};
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
        calories.push_back(currentCalories);
    }
    fmt::print("\n");
    std::sort(calories.begin(), calories.end(),
              [](auto a, auto b) { return a > b; });
    fmt::print("Any elf carries at most {} calories.\n", calories[0]);
    fmt::print("The top 3 elves carry a total of {} calories.\n",
               calories[0] + calories[1] + calories[2]);
}
