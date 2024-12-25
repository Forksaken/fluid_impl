#pragma once

#include <limits>

#include "Types.h"

using namespace std;

class Float {
public:
    constexpr Float(int v) : v(v) {}
    constexpr Float(float f) : v(f) {}
    constexpr Float(double f) : v(static_cast<float>(f)) {}
    constexpr Float() : v(0) {}

        template <typename OTHER> constexpr Float(OTHER x) : v(to_double(x)) {}

        float v;

        auto operator<=>(const Float &) const = default;
        bool operator==(const Float &) const = default;

        template <typename OTHER> Float &operator=(OTHER x)    {
                v = to_double(x);
                return *this;
        }

        template <typename OTHER> auto operator<=>(OTHER x) const {
                return v <=> to_double(x);
        }

        template <typename OTHER> bool operator==(OTHER x) const {
                return v == to_double(x);
        }

        static constexpr Float get_epsilon() {
                return Float(4);
        }

        static constexpr Float get_infinity() {
                return Float(std::numeric_limits<float>::max());
        }

        static Float random01() {
                uint64_t r = rand()%1'000'000'000;
                if (r < 0)
                        r = -r;
                return Float((double)r / 1'000'000'000);
        }
};

float to_double(Float x) {
    return x.v;
}

template <typename OTHER> Float operator+(Float a, OTHER b) {
    return a + Float(to_double(b));
}

template<>
Float operator+(Float a, Float b) {
    return Float(a.v + b.v);
}

Float operator-(Float x) {
    return Float(-x.v);
}

template <typename OTHER> Float operator-(Float a, OTHER b) {
    return a - Float(to_double(b));
}

template<>
Float operator-(Float a, Float b) {
    return Float(a.v - b.v);
}

template <typename OTHER> Float operator*(Float a, OTHER b) {
    return a * Float(to_double(b));
}

template<>
Float operator*(Float a, Float b) {
    return Float(a.v * b.v);
}

template <typename OTHER> Float operator/(Float a, OTHER b) {
    return a / Float(to_double(b));
}

template<>
Float operator/(Float a, Float b) {
    return Float(a.v / b.v);
}

template <typename OTHER> Float &operator+=(Float &a, OTHER b) {
    return a = a + Float(to_double(b));
}

template<>
Float &operator+=(Float &a, Float b) {
    return a = a + b;
}

template <typename OTHER> Float &operator-=(Float &a, OTHER b) {
    return a = a - Float(to_double(b));
}

template<>
Float &operator-=(Float &a, Float b) {
    return a = a - b;
}

template <typename OTHER> Float &operator*=(Float &a, OTHER b) {
    return a = a * Float(to_double(b));
}

template<>
Float &operator*=(Float &a, Float b) {
    return a = a * b;
}

template <typename OTHER> Float &operator/=(Float &a, OTHER b) {
    return a = a / Float(to_double(b));
}

template<>
Float &operator/=(Float &a, Float b) {
    return a = a / b;
}

Float abs(Float x) {
    return Float(std::abs(x.v));
}

ostream &operator<<(ostream &os, Float x) {
    os << x.v;
    return os;
}