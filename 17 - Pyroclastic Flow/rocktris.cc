#include <algorithm>
#include <fmt/format.h>
#include <fstream>
#include <iostream>
#include <ranges>
#include <string>
#include <vector>

#include "vec2.hpp"

using std::views::iota;

struct Jets;
struct Rock;
struct Shape;
struct Tower;

struct Tower {
    std::vector<std::string> pile{};
    int64_t nextShape{0};

    Rock spawnRock();
    void addRock(const Rock &rock);
    int64_t height() const;
    bool isSolid(const Vec2l &pos) const;
    void print() const;
    void print(const Rock &current) const;
};

struct Shape {
    const std::vector<std::string> s;

    int64_t width() const { return s.at(0).size(); }

    int64_t height() const { return s.size(); }

    bool isSolid(const Vec2l &pos) const {
        // fmt::print("Shape::isSolid({})\n",pos);
        // fmt::print("  w={},h={}\n",width(),height());
        return s.at(pos.y)[pos.x] == '#';
    }
};

static const std::vector<Shape> shapes{{{{"####"}},
                                        {{".#.", "###", ".#."}},
                                        {{"###", "..#", "..#"}},
                                        {{"#", "#", "#", "#"}},
                                        {{"##", "##"}}}};

struct Jets {
    const std::string &pattern;
    size_t position{};
    Jets(std::string &in) : pattern(in), position(in.size()) {}
    int64_t direction() {
        if (++position >= pattern.size()) {
            position = 0;
        }
        switch (pattern[position]) {
        case '<':
            return -1;
        case '>':
            return 1;
        default:
            ++position;
            return direction();
        }
    }
};

struct Rock {
    Tower &tower;
    Vec2l pos{}; // bottom left
    int64_t shape;
    bool falling{};

    Rock(Tower &tower, const Vec2l &pos, int64_t shape)
        : tower(tower), pos(pos), shape(shape), falling(true) {}

    bool isFalling() const { return falling; }

    bool canMove(const Vec2l &dest) const {
        for (const auto x : iota(0, width())) {
            for (const auto y : iota(0, height())) {
                if (tower.isSolid(dest + Vec2l{x, y}) and
                    this->isSolid(pos + Vec2l{x, y})) {
                    return false;
                }
            }
        }
        return true;
    }

    void blow(int64_t dir) {
        if (canMove(pos + Vec2l{dir, 0})) {
            pos += Vec2l{dir, 0};
        }
    }

    void drop() {
        if (canMove(pos + Vec2l{0, -1})) {
            pos += Vec2l{0, -1};
        } else {
            falling = false;
            tower.addRock(*this);
        }
    }

    int64_t width() const { return shapes.at(shape).width(); }

    int64_t height() const { return shapes.at(shape).height(); }

    bool isSolid(const Vec2l &loc) const {
        // fmt::print("Rock::isSolid({}) @{}\n",loc,pos);
        const auto rPos = loc - pos;
        if (rPos.x < 0 or rPos.y < 0 or rPos.x >= width() or
            rPos.y >= height()) {
            return false;
        }
        return shapes.at(shape).isSolid(loc - pos);
    }
};

Rock Tower::spawnRock() {
    nextShape %= 5;
    return Rock(*this, {2, height() + 3}, nextShape++);
}

void Tower::addRock(const Rock &rock) {
    for (const auto y : iota(rock.pos.y, rock.pos.y + rock.height())) {
        for (const auto x : iota(rock.pos.x, rock.pos.x + rock.width())) {
            if (rock.isSolid({x, y})) {
                while (y >= (int64_t)pile.size()) {
                    pile.push_back(".......");
                }
                pile[y][x] = '#';
            }
        }
    }
}

int64_t Tower::height() const { return pile.size(); }

bool Tower::isSolid(const Vec2l &pos) const {
    // fmt::print("Tower::isSolid({})\n",pos);
    if (pos.y < 0) {
        return true;
    }
    if (pos.x < 0 or pos.x > 6) {
        return true;
    }
    if (pos.y < height()) {
        return pile[pos.y][pos.x] == '#';
    } else {
        return false;
    }
}

void Tower::print() const {
    const Rock fake{const_cast<Tower &>(*this), {-10, -10}, 0};
    print(fake);
}

void Tower::print(const Rock &current) const {
    const auto maxY = std::max(height(), current.pos.y + current.height());
    for (const auto y :
         iota(-maxY, 1) | std::views::transform([](auto x) { return -x; })) {
        std::cout << '|';
        for (const auto x : iota(0, 7)) {
            if (current.isSolid({x, y})) {
                if (current.isFalling()) {
                    std::cout << '@';
                } else {
                    std::cout << '=';
                }
            } else {
                if (this->isSolid({x, y})) {
                    std::cout << '#';
                } else {
                    std::cout << '.';
                }
            }
        }
        std::cout << "|\n";
    }
    std::cout << "+-------+\n";
    std::cout << "Size: " << height() << "\n\n";
}

int64_t level1(std::string &input) {
    Tower tower{};
    Jets jets{input};
    int64_t fallenRocks = 0;
    while (fallenRocks < 2022) {
        auto rock = tower.spawnRock();
        // tower.print(rock);
        while (rock.isFalling()) {
            rock.blow(jets.direction());
            rock.drop();
        }
        ++fallenRocks;
    }
    tower.print();
    return tower.height();
}

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input.txt>\n";
        std::exit(EXIT_FAILURE);
    }

    std::ifstream infile{argv[1]};
    std::string line;
    std::getline(infile, line);
    fmt::print("The 2022 rocks tower is {} tall\n", level1(line));
}
