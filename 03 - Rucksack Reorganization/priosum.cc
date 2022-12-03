#include <fmt/format.h>
#include <fstream>
#include <ranges>
#include <set>

using std::views::iota;

int64_t itemPrio(char item) {
    if (item >= 'a' and item <= 'z') {
        return item - 'a' + 1;
    }
    if (item >= 'A' and item <= 'Z') {
        return item - 'A' + 27;
    }
    fmt::print("invalid item {}\n", item);
    return -1;
}

int main(int, char **argv) {
    std::ifstream infile{argv[1]};
    std::string rucksack;

    int64_t prioSum = 0;

    while (std::getline(infile, rucksack)) {
        std::set<char> front{};
        for (const auto i : iota(0u, rucksack.size() / 2)) {
            front.insert(rucksack[i]);
        }
        for (const auto i : iota(rucksack.size() / 2, rucksack.size())) {
            if (front.contains(rucksack[i])) {
                front.erase(rucksack[i]);
                prioSum += itemPrio(rucksack[i]);
            }
        }
    }
    fmt::print("The total prioriy is {}.\n", prioSum);
}
