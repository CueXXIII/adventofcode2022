#include <fmt/format.h>
#include <fstream>
#include <ranges>
#include <set>
#include <string>

using std::views::iota;

void tuneIn(const std::string &message) {
    for (const auto marker : iota(0u, message.size() - 4)) {
        std::set<char> sent{};
        for (const auto index : iota(0, 4)) {
            sent.insert(message[marker + index]);
        }
        if (sent.size() == 4) {
            fmt::print("Marker {} found after {} characters.\n",
                       message.substr(marker, 4), marker + 4);
            return;
        }
    }
    fmt::print("Synchroonization failed\n");
}

int main(int, char **argv) {
    std::ifstream infile{argv[1]};
    std::string line;
    while (std::getline(infile, line)) {
        tuneIn(line);
    }
}
