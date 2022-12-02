#include <fmt/format.h>
#include <fstream>
#include <string>
#include <unordered_map>

const std::unordered_map<std::string, int32_t> strategyScore{
    {"B X", 1}, {"C Y", 2}, {"A Z", 3}, {"A X", 4}, {"B Y", 5},
    {"C Z", 6}, {"C X", 7}, {"A Y", 8}, {"B Z", 9}};

int main(int, char **argv) {
    std::ifstream infile{argv[1]};

    int32_t totalScore = 0;
    std::string line;
    while (std::getline(infile, line)) {
        totalScore += strategyScore.at(line);
    }
    fmt::print("Your total score according to the strategy guide is {}\n",
               totalScore);
}
