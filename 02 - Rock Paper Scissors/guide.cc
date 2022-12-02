#include <fmt/format.h>
#include <fstream>
#include <string>
#include <unordered_map>

const std::unordered_map<std::string, int32_t> strategyScore{
    {"A X", 4}, {"B X", 1}, {"C X", 7}, {"A Y", 8}, {"B Y", 5},
    {"C Y", 2}, {"A Z", 3}, {"B Z", 9}, {"C Z", 6}};

const std::unordered_map<std::string, int32_t> strategy2Score{
    {"A X", 3}, {"B X", 1}, {"C X", 2}, {"A Y", 4}, {"B Y", 5},
    {"C Y", 6}, {"A Z", 8}, {"B Z", 9}, {"C Z", 7}};

int main(int, char **argv) {
    std::ifstream infile{argv[1]};

    int32_t totalScore = 0;
    int32_t playedScore = 0;
    std::string line;
    while (std::getline(infile, line)) {
        totalScore += strategyScore.at(line);
        playedScore += strategy2Score.at(line);
    }
    fmt::print("Your total score according to the strategy guide is {}\n",
               totalScore);
    fmt::print("Your revised strategy gives you a score of {}\n", playedScore);
}
