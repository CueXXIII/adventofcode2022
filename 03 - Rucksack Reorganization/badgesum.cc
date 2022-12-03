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

void searchFirst(const std::string &rucksack, std::set<char> &badge) {
    for (const auto &item : rucksack) {
        badge.insert(item);
    }
}

void clearNonBadge(const std::string &rucksack, std::set<char> &badge) {
    std::set<char> common{};
    for (const auto &item : rucksack) {
        if (badge.contains(item)) {
            common.insert(item);
        }
    }
    badge = common;
}

int main(int, char **argv) {
    std::ifstream infile{argv[1]};
    std::string rucksack;

    int64_t badgeSum = 0;

    while (std::getline(infile, rucksack)) {
        std::set<char> badge{};
        searchFirst(rucksack, badge);

        std::getline(infile, rucksack);
        clearNonBadge(rucksack, badge);

        std::getline(infile, rucksack);
        clearNonBadge(rucksack, badge);

        for (const auto &item : badge) {
            badgeSum += itemPrio(item);
        }
    }
    fmt::print("The badges' prioriy is {}.\n", badgeSum);
}
