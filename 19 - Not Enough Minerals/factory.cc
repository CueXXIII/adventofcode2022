#include <algorithm>
#include <fmt/format.h>
#include <fstream>
#include <iostream>
#include <limits>
#include <ranges>
#include <string>
#include <vector>

#include "simpleparser.hpp"

using std::views::iota;

int64_t geodesMonitor = 0;

enum Material { ore = 0, clay = 1, obsidian = 2, geode = 3, none = -1 };

struct Blueprint {
    int64_t id{};
    std::array<int64_t, 6> costs{};
    std::array<int64_t, 4> maxBot{};

    Blueprint(const int id, const std::array<int64_t, 6> &costs)
        : id(id), costs(costs) {
        maxBot[ore] = std::max({costs[0], costs[1], costs[2], costs[4]});
        maxBot[clay] = costs[3];
        maxBot[obsidian] = costs[5];
        maxBot[geode] = std::numeric_limits<int64_t>::max();
    }

    std::array<int64_t, 3> buildCost(Material robot) const {
        switch (robot) {
        case ore:
            return {costs[0], 0, 0};
        case clay:
            return {costs[1], 0, 0};
        case obsidian:
            return {costs[2], costs[3], 0};
        case geode:
            return {costs[4], 0, costs[5]};
        default:
            return {0, 0, 0};
        }
    }

    friend std::ostream &operator<<(std::ostream &out, const Blueprint &print) {
        return out << "Blueprint " << print.id << ": \n";
    }
};

struct World {
    std::array<int64_t, 4> robots{1, 0, 0, 0};
    std::array<int64_t, 4> materials{0, 0, 0, 0};

    bool canBuild(const Blueprint &print, const Material bot) const {
        if (bot == none) {
            return true;
        }
        if (robots[bot] >= print.maxBot[bot]) {
            // don't build more bots than needed
            return false;
        }
        const auto cost = print.buildCost(bot);
        for (const auto m : {ore, clay, obsidian}) {
            if (materials[m] < cost[m]) {
                return false;
            }
        }
        return true;
    }

    void startBuild(const Blueprint &print, const Material bot) {
        if (bot == none) {
            return;
        }
        const auto cost = print.buildCost(bot);
        for (const auto m : {ore, clay, obsidian}) {
            materials[m] -= cost[m];
        }
    }

    bool supplyCompleted(const Blueprint &print) const {
        return robots[ore] == print.maxBot[ore] and
               robots[clay] == print.maxBot[clay] and
               robots[obsidian] == print.maxBot[obsidian] and
               canBuild(print, geode);
    }

    friend std::ostream &operator<<(std::ostream &out, const World &world) {
        out << fmt::format("  Collecting bots: {}, {}, {}, {}\n",
                           world.robots[0], world.robots[1], world.robots[2],
                           world.robots[3]);
        out << fmt::format("  Material stock:  {}, {}, {}, {}\n",
                           world.materials[0], world.materials[1],
                           world.materials[2], world.materials[3]);
        return out;
    }
};

std::vector<Blueprint> blueprints;

int64_t geodeAmount(const Blueprint &print, int64_t minutesLeft, World world,
                    const Material buildingBot) {
    // debug
    // std::cout << "World with " << minutesLeft << " minutes left:\n  building
    // "
    //          << buildingBot << "\n"
    //          << world;

    // change world according to the choices made
    if (buildingBot != none) {
        world.startBuild(print, buildingBot);
    }
    for (const auto mat : {ore, clay, obsidian, geode}) {
        world.materials[mat] += world.robots[mat];
    }
    if (buildingBot != none) {
        ++world.robots[buildingBot];
    }
    --minutesLeft;

    // check for end of time
    if (minutesLeft == 0) {
        if (geodesMonitor < world.materials[geode]) {
            //fmt::print("New max geodes: {}\n", world.materials[geode]);
            geodesMonitor = world.materials[geode];
        }
        return world.materials[geode];
    } else if (minutesLeft == 1) {
        // doesn't matter what we build in the last minute, either
        return geodeAmount(print, 1, world, none);
    }

    // check if we should abort early
    //
    // assume one geode bot every round
    const auto canHarvestGeodes =
        world.robots[geode] * minutesLeft + (minutesLeft - 1) * minutesLeft / 2;
    if (world.materials[geode] + canHarvestGeodes < geodesMonitor) {
        return 0;
    }

    // think about next turn
    int64_t maxGeodes = 0;
    if (world.supplyCompleted(print)) {
        return geodeAmount(print, minutesLeft, world, geode);
    }
    for (const Material bot : {geode, obsidian, clay, none, ore}) {
        if (world.canBuild(print, bot)) {
            maxGeodes = std::max(maxGeodes,
                                 geodeAmount(print, minutesLeft, world, bot));
        }
    }
    return maxGeodes;
}

int64_t geodeAmount(const Blueprint &print, int64_t minutes) {
    fmt::print("Blueprint {}\n", print.id);
    geodesMonitor = 0;
    // turn 1 does not require thinking, we can't build anything yet
    return geodeAmount(print, minutes, {}, none);
}

int64_t qualityLevel(const Blueprint &print) {
    return geodeAmount(print, 24) * print.id;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input.txt>\n";
        std::exit(EXIT_FAILURE);
    }

    SimpleParser scanner{argv[1]};
    while (!scanner.isEof()) {
        auto v = scanner.skipToken("Blueprint");
        const auto id = scanner.getInt64();
        v |= scanner.skipToken(": Each ore robot costs");
        const auto c0 = scanner.getInt64();
        v |= scanner.skipToken("ore. Each clay robot costs");
        const auto c1 = scanner.getInt64();
        v |= scanner.skipToken("ore. Each obsidian robot costs");
        const auto c2 = scanner.getInt64();
        v |= scanner.skipToken("ore and");
        const auto c3 = scanner.getInt64();
        v |= scanner.skipToken("clay. Each geode robot costs");
        const auto c4 = scanner.getInt64();
        v |= scanner.skipToken("ore and");
        const auto c5 = scanner.getInt64();
        v |= scanner.skipToken("obsidian.");
        if (v) {
            blueprints.emplace_back(
                id, std::array<int64_t, 6>{c0, c1, c2, c3, c4, c5});
        } else {
            fmt::print("Error: misformed blueprint {}\n", id);
        }
    }

    int64_t sum = 0;
    for (const auto &blueprint : blueprints) {
        sum += qualityLevel(blueprint);
    }
    fmt::print("The total quality level is {}\n", sum);

    int64_t prod = 1;
    for (const auto i : iota(0, 3)) {
        prod *= geodeAmount(blueprints[i], 32);
    }
    fmt::print("The product of the first 3 blueprints is {}\n", prod);
}
