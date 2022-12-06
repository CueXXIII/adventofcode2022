#include <fmt/format.h>
#include <fstream>
#include <ranges>
#include <set>
#include <string>

using std::views::iota;

void tuneIn(const std::string &message, const size_t length,
            const std::string &what) {
    for (const auto marker : iota(0u, message.size() - length)) {
        std::set<char> sent{};
        for (const auto index : iota(0u, length)) {
            sent.insert(message[marker + index]);
        }
        if (sent.size() == length) {
            fmt::print("Start of {} \"{}\" found after {} characters.\n", what,
                       message.substr(marker, length), marker + length);
            return;
        }
    }
    fmt::print("Synchroonization failed\n");
}

int main(int, char **argv) {
    std::ifstream infile{argv[1]};
    std::string line;
    while (std::getline(infile, line)) {
        tuneIn(line, 4, "start-of-packet");
        tuneIn(line, 14, "start-of-message");
        fmt::print("\n");
    }
}
