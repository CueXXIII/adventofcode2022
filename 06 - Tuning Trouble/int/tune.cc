#include <chrono>
#include <fmt/format.h>
#include <fstream>
#include <queue>
#include <ranges>
#include <unordered_map>
#include <vector>

using std::views::iota;

struct Window {
    int64_t charCount;
    int64_t repeatedCount;
    const int64_t windowSize;
    std::vector<int64_t> bucket{};

    Window(const int64_t size)
        : charCount(0), repeatedCount(0), windowSize(size) {
        bucket.resize(1000000);
    }

    void push(auto &message, const int64_t position) {
        ++charCount;
        const auto value = message[position];
        // count value in bucket
        bucket[value] += 1;
        if (bucket[value] == 2) {
            ++repeatedCount;
        }
        if (charCount <= windowSize) {
            // window is not full, yet
            return;
        }
        // remove first window value from bucket
        const auto removeValue = message[position - windowSize];
        bucket[removeValue] -= 1;
        if (bucket[removeValue] == 1) {
            --repeatedCount;
        }
    }

    bool isUnique() const { return repeatedCount == 0; }
};

int main(int argc, char **argv) {
    auto startTime = std::chrono::high_resolution_clock::now();

    if (argc != 3) {
        fmt::print("Usage: {} inputfile prefix_length\n", argv[0]);
        std::exit(EXIT_FAILURE);
    }

    std::ifstream infile{argv[1]};
    const int64_t prefixLength = std::stol(argv[2]);

    Window window{prefixLength};

    std::vector<int64_t> message;
    std::string line;
    while (std::getline(infile, line)) {
        message.push_back(std::stol(line));
    }

    auto parse = std::chrono::high_resolution_clock::now();
    
    for (const auto position : iota(0, prefixLength-1)) {
        window.push(message, position);
    }
    for (const auto position : iota(prefixLength-1, (int64_t)message.size())) {
        window.push(message, position);
        if (window.isUnique()) {
            fmt::print("Unique marker at {}\n", window.charCount);
            break;
        }
    }

    auto solve = std::chrono::high_resolution_clock::now();

    fmt::print("Time for parsing: {} ms\n", std::chrono::duration_cast<std::chrono::milliseconds>(parse-startTime).count());
    fmt::print("Time for solving: {} ms\n", std::chrono::duration_cast<std::chrono::milliseconds>(solve-parse).count());
}
