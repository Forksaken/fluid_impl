#pragma once

#include <cstdint>
#include <iostream>
#include <type_traits>

using namespace std;

template <size_t S, size_t K>
class Fixed {
public:
    using RawType = std::conditional_t<S <= 32, int32_t, int64_t>;
    static constexpr size_t Bits = S;
    static constexpr size_t Fraction = K;
    RawType v;

    constexpr Fixed(int val) : v(val << S) {}
    constexpr Fixed(float f) : v(static_cast<RawType>(f * (1 << S))) {}
    constexpr Fixed(double d) : v(static_cast<RawType>(d * (1 << S))) {}
    constexpr Fixed() : v(0) {}

    template <typename OTHER>
    constexpr Fixed(OTHER x) : v(static_cast<RawType>(x * (1 << S))) {}

    static constexpr Fixed from_raw(RawType raw) {
        Fixed tmp;
        tmp.v = raw;
        return tmp;
    }

    auto operator<=>(const Fixed &) const = default;
    bool operator==(const Fixed &) const = default;

    template <typename OTHER>
    Fixed& operator=(OTHER x) {
        v = static_cast<RawType>(x * (1 << S));
        return *this;
    }

    explicit operator float() const {
        return static_cast<float>(v) / (1 << S);
    }

    explicit operator double() const {
        return static_cast<double>(v) / (1 << S);
    }

    template <typename IntType, typename = enable_if_t<is_integral_v<IntType>>>
    explicit operator IntType() const {
        return static_cast<IntType>(v >> S);
    }

    Fixed operator+(const Fixed& other) const {
        Fixed tmp;
        tmp.v = v + other.v;
        return tmp;
    }

    Fixed operator-(const Fixed& other) const {
        Fixed tmp;
        tmp.v = v - other.v;
        return tmp;
    }

    Fixed operator*(const Fixed& other) const {
        using Intermediate = int64_t;
        Intermediate product = static_cast<Intermediate>(v) * static_cast<Intermediate>(other.v);
        Fixed tmp;
        tmp.v = static_cast<RawType>(product >> S);
        return tmp;
    }

    Fixed operator/(const Fixed& other) const {
        using Intermediate = int64_t;
        Intermediate divRes = (static_cast<Intermediate>(v) << S) / other.v;
        Fixed tmp;
        tmp.v = static_cast<RawType>(divRes);
        return tmp;
    }

    Fixed& operator+=(const Fixed& rhs) {
        v += rhs.v;
        return *this;
    }

    Fixed& operator-=(const Fixed& rhs) {
        v -= rhs.v;
        return *this;
    }

    Fixed& operator*=(const Fixed& rhs) {
        *this = *this * rhs;
        return *this;
    }

    Fixed& operator/=(const Fixed& rhs) {
        *this = *this / rhs;
        return *this;
    }

    friend ostream& operator<<(ostream& os, const Fixed& val) {
        os << static_cast<double>(val);
        return os;
    }
};

template <size_t N, size_t K, typename OTHER>
Fixed<N, K> operator+(const Fixed<N, K>& a, OTHER b) {
    return a + Fixed<N, K>(b);
}

template <size_t N, size_t K, typename OTHER>
Fixed<N, K> operator-(const Fixed<N, K>& a, OTHER b) {
    return a - Fixed<N, K>(b);
}

template <size_t N, size_t K, typename OTHER>
Fixed<N, K> operator*(const Fixed<N, K>& a, OTHER b) {
    return a * Fixed<N, K>(b);
}

template <size_t N, size_t K, typename OTHER>
Fixed<N, K> operator/(const Fixed<N, K>& a, OTHER b) {
    return a / Fixed<N, K>(b);
}