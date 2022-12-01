#pragma once

#include <fstream>

#include "vec2.hpp"

template <typename num> struct Vec3 {
    num x{};
    num y{};
    num z{};

    constexpr Vec3() noexcept = default;
    constexpr Vec3(const num &x, const num &y, const num &z) noexcept
        : x(x), y(y), z(z) {}
    constexpr Vec3(const Vec2<num> &v, num z) noexcept : x(v.x), y(v.y), z(z) {}

    bool operator==(const Vec3 &other) const {
        return x == other.x && y == other.y && z == other.z;
    }
    Vec3 &operator+=(const Vec3 &other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }
    Vec3 &operator-=(const Vec3 &other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }
    Vec3 &operator*=(const num factor) {
        x *= factor;
        y *= factor;
        z *= factor;
        return *this;
    }

    Vec3 operator+(const Vec3 &other) const { return Vec3{*this} += other; }
    Vec3 operator-(const Vec3 &other) const { return Vec3{*this} -= other; }
    Vec3 operator*(const num factor) const { return Vec3{*this} *= factor; }

    friend std::ostream &operator<<(std::ostream &out, const Vec3 &vec) {
        return out << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
    }
};

template <typename num> struct std::hash<Vec3<num>> {
    std::size_t operator()(const Vec3<num> &v) const noexcept {
        return std::hash<num>{}(v.x) * 2 + std::hash<num>{}(v.y) * 1627 +
               std::hash<num>{}(v.z) * 2642257;
    }
};

using Vec3i = Vec3<int32_t>;
using Vec3l = Vec3<int64_t>;
using Vec3z = Vec3<size_t>;
using Vec3f = Vec3<double>;
