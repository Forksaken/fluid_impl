#pragma once

#include "Types.h"

using namespace std;

class Double {
    public:
        constexpr Double(int v) : v(v) {}
        constexpr Double(float f) : v(f) {}
        constexpr Double(double f) : v(f) {}
        constexpr Double() : v(0) {}

        template <typename OTHER> constexpr Double(OTHER x) : v(to_double(x)) {}

        double v;

        auto operator<=>(const Double &) const = default;
        bool operator==(const Double &) const = default;

        template <typename OTHER> Double &operator=(OTHER x) {
            v = to_double(x);
            return *this;
        }

        template <typename OTHER> auto operator<=>(OTHER x) const {
            return v <=> to_double(x);
        }

        template <typename OTHER> bool operator==(OTHER x) const {
            return v == to_double(x);
        }

        static constexpr Double get_epsilon() {
            return Double(4);
        }

        static constexpr Double get_infinity() {
            return Double(std::numeric_limits<double>::max());
        }

        static Double random01() {
            uint64_t r = rand()%1'000'000'000;
            if (r < 0)
                r = -r;
            return Double((double)r / 1'000'000'000);
        }
};

double to_double(Double x) {
    return x.v;
}

template <typename OTHER> Double operator+(Double a, OTHER b) {
    return a + Double(to_double(b));
}

template<>
Double operator+(Double a, Double b) {
    return Double(a.v + b.v);
}

Double operator-(Double x) {
    return Double(-x.v);
}

template <typename OTHER> Double operator-(Double a, OTHER b) {
    return a - Double(to_double(b));
}

template<>
Double operator-(Double a, Double b) {
    return Double(a.v - b.v);
}

template <typename OTHER> Double operator*(Double a, OTHER b) {
    return a * Double(to_double(b));
}

template<>
Double operator*(Double a, Double b) {
    return Double(a.v * b.v);
}

template <typename OTHER> Double operator/(Double a, OTHER b) {
    return a / Double(to_double(b));
}

template<>
Double operator/(Double a, Double b) {
    return Double(a.v / b.v);
}

template <typename OTHER> Double &operator+=(Double &a, OTHER b) {
    return a = a + Double(to_double(b));
}

template<>
Double &operator+=(Double &a, Double b) {
    return a = a + b;
}

template <typename OTHER> Double &operator-=(Double &a, OTHER b) {
    return a = a - Double(to_double(b));
}

template<>
Double &operator-=(Double &a, Double b) {
    return a = a - b;
}

template <typename OTHER> Double &operator*=(Double &a, OTHER b) {
    return a = a * Double(to_double(b));
}

template<>
Double &operator*=(Double &a, Double b) {
    return a = a * b;
}

template <typename OTHER> Double &operator/=(Double &a, OTHER b) {
    return a = a / Double(to_double(b));
}

template<>
Double &operator/=(Double &a, Double b) {
    return a = a / b;
}

Double abs(Double x) {
    return Double(std::abs(x.v));
}

ostream &operator<<(ostream &os, Double x) {
    os << x.v;
    return os;
}