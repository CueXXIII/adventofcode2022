#pragma once

#include <fstream>

template <typename num> struct Vec2 {
    num x{};
    num y{};

    constexpr Vec2() noexcept = default;
    constexpr Vec2(const num &x, const num &y) noexcept : x(x), y(y) {}

    bool operator==(const Vec2 &other) const {
        return x == other.x && y == other.y;
    }
    Vec2 &operator+=(const Vec2 &other) {
        x += other.x;
        y += other.y;
        return *this;
    }
    Vec2 &operator-=(const Vec2 &other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }
    Vec2 &operator*=(const num factor) {
        x *= factor;
        y *= factor;
        return *this;
    }

    Vec2 operator+(const Vec2 &other) const { return Vec2{*this} += other; }
    Vec2 operator-(const Vec2 &other) const { return Vec2{*this} -= other; }
    Vec2 operator*(const num factor) const { return Vec2{*this} *= factor; }

    friend std::ostream &operator<<(std::ostream &out, const Vec2 &vec) {
        return out << "(" << vec.x << ", " << vec.y << ")";
    }
};

template <typename num> struct std::hash<Vec2<num>> {
    std::size_t operator()(const Vec2<num> &v) const noexcept {
        return std::hash<int64_t>{}((static_cast<int64_t>(v.x) << 16 ^
                                     static_cast<int64_t>(v.x) >> 48) ^
                                    static_cast<int64_t>(v.y));
    }
};

using Vec2i = Vec2<int32_t>;
using Vec2l = Vec2<int64_t>;
using Vec2z = Vec2<size_t>;
using Vec2f = Vec2<double>;
