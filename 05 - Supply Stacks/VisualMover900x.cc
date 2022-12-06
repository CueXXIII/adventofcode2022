#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <fmt/format.h>
#include <fstream>
#include <ranges>
#include <vector>

#include "simpleparser.hpp"

using std::views::iota;

class Storage;
class Crane;

void startAnimation() { fmt::print("\x1b[H\x1b[2J\x1b[3J"); }

void startAnimFrame() { fmt::print("\x1b[H"); }

void delayAnimation() { usleep(1000 * 1000 / 60); }

class Storage {
  private:
    mutable size_t maxStorageHeight = 47; // 47 for my input

  protected:
    std::vector<std::vector<char>> hold{};

  public:
    void insertBottom(size_t position, char crate) {
        if (hold.size() <= position) {
            hold.resize(position + 1);
        }
        hold[position].insert(hold[position].begin(), crate);
    }

    void printStorage() const { printStorage(0, 0, {}, ""); }

    void printStorage(const size_t carryX, const size_t carryY,
                      const std::vector<char> carry,
                      const std::string moveDescription) const {
        size_t maxStack = 0;
        for (const auto &stack : hold) {
            maxStack = std::max(maxStack, stack.size());
        }
        maxStorageHeight = std::max({maxStorageHeight, maxStack, carryY + 1});
        for (const auto pass : iota(0u, maxStorageHeight)) {
            const auto pos = maxStorageHeight - pass - 1;
            for (const auto stackpos : iota(0u, hold.size())) {
                const auto &stack = hold[stackpos];
                if (pos <= carryY and pos + carry.size() > carryY and
                    stackpos == (carryX + 3) / 4) {
                    const auto offset = carryX % 4;
                    if (offset == 0) {
                        fmt::print("[{}] ", carry[carryY - pos]);
                    } else {
                        fmt::print("\x1b[{}D[{}] \x1b[{}C", (4 - offset) % 4,
                                   carry[carryY - pos], (4 - offset) % 4);
                    }
                } else if (stack.size() > pos) {
                    fmt::print("[{}] ", stack.at(pos));
                } else {
                    fmt::print("    ");
                }
            }
            fmt::print("\n");
        }
        for (const auto num : iota(0u, hold.size())) {
            fmt::print(" {}  ", num + 1);
        }
        fmt::print("\n\x1b[K\n");
        fmt::print("\x1b[K{}\n\n", moveDescription);
    };

    void printTopCrates() const {
        for (const auto &stack : hold) {
            fmt::print("{}", stack.back());
        }
        fmt::print("\n");
    }

    friend class Crane;
};

class Crane {
  protected:
    Storage &storage;
    decltype(storage.hold) &hold;

    void animateMove(const std::vector<char> &crates, const size_t src,
                     const size_t dst, const std::string moveDescription) {
        const auto crateHeight = crates.size();
        const auto srcHeight = hold[src].size();
        const auto dstHeight = hold[dst].size();

        // move the crate at least 1 above all stacks
        auto liftHeight = srcHeight + crateHeight;
        // look at all remaining positions but src (in reverse)
        for (size_t position = dst; position != src;
             position += (dst < src) ? 1 : -1) {
            liftHeight =
                std::max(liftHeight, hold[position].size() + crateHeight);
        }

        // animate the transport
        size_t xPos = src * 4;
        size_t yPos = srcHeight + crateHeight;
        // up
        for (; yPos < liftHeight; ++yPos) {
            startAnimFrame();
            storage.printStorage(xPos, yPos, crates, moveDescription);
            delayAnimation();
        }
        // horizontal
        for (; xPos != dst * 4; xPos += (src < dst) ? 1 : -1) {
            startAnimFrame();
            storage.printStorage(xPos, yPos, crates, moveDescription);
            delayAnimation();
        }
        // down
        for (; yPos >= dstHeight + crateHeight; --yPos) {
            startAnimFrame();
            storage.printStorage(xPos, yPos, crates, moveDescription);
            delayAnimation();
        }
        startAnimFrame();
        storage.printStorage(xPos, yPos, crates, moveDescription);
        delayAnimation();
    }

  public:
    Crane(Storage &s) : storage(s), hold(storage.hold) {}
    virtual void move(size_t, size_t, size_t) = 0;
};

class CrateMover9000 : public Crane {
  public:
    CrateMover9000(Storage &s) : Crane(s) {}
    virtual void move(size_t amount, size_t src, size_t dst) override {
        for ([[maybe_unused]] const auto i : iota(0u, amount)) {
            // get crate
            const auto crate = hold[src].back();
            hold[src].pop_back();

            animateMove(
                {crate}, src, dst,
                fmt::format("move {} from {} to {}", amount, src + 1, dst + 1));

            // put crate back
            hold[dst].push_back(crate);
        }
    }
};

class CrateMover9001 : public Crane {
  public:
    CrateMover9001(Storage &s) : Crane(s) {}
    virtual void move(size_t amount, size_t src, size_t dst) override {
        // get crate
        std::vector<char> lift{};
        for ([[maybe_unused]] const auto i : iota(0u, amount)) {
            lift.push_back(hold[src].back());
            hold[src].pop_back();
        }

        animateMove(
            lift, src, dst,
            fmt::format("move {} from {} to {}", amount, src + 1, dst + 1));

        // put crate back
        for ([[maybe_unused]] const auto i : iota(0u, amount)) {
            hold[dst].push_back(lift.back());
            lift.pop_back();
        }
    }
};

int main(int argc, char **argv) {
    if (argc != 3) {
        fmt::print("Usage: {} inputfile 9000|9001\n", argv[0]);
        std::exit(EXIT_FAILURE);
    }
    const auto version = std::stol(argv[2]);

    Storage ship{};

    std::ifstream infile{argv[1]};
    std::string line;

    while (std::getline(infile, line) and line.size() > 0) {
        const auto numCrates = (line.size() + 1) / 4;
        for (const auto i : iota(0u, numCrates)) {
            if (line[i * 4] == '[') {
                ship.insertBottom(i, line[i * 4 + 1]);
            }
        };
    }

    startAnimation();
    startAnimFrame();
    ship.printStorage();
    delayAnimation();

    Crane *crane = nullptr;
    switch (version) {
    case 9000:
        crane = new CrateMover9000(ship);
        break;
    case 9001:
        crane = new CrateMover9001(ship);
        break;
    default:
        fmt::print("Usage: {} inputfile 9000|9001\n", argv[0]);
        std::exit(EXIT_FAILURE);
    }

    SimpleParser moves{infile};
    while (!moves.isEof()) {
        moves.skipToken("move");
        const auto num = moves.getInt64();
        moves.skipToken("from");
        const auto src = moves.getInt64() - 1;
        moves.skipToken("to");
        const auto dst = moves.getInt64() - 1;

        crane->move(num, src, dst);
    }
}
