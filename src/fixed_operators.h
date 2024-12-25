#pragma once

#include "Fixed.h"
#include "FastFixed.h"

template<size_t S, size_t K>
Fixed<S, K> operator+(const Fixed<S, K>& a, const FastFixed<S, K>& b) {
    return a + Fixed<S, K>(static_cast<double>(b));
}

template<size_t S, size_t K>
Fixed<S, K> operator+(const FastFixed<S, K>& a, const Fixed<S, K>& b) {
    return Fixed<S, K>(static_cast<double>(a)) + b;
}

template<size_t S, size_t K>
Fixed<S, K> operator-(const Fixed<S, K>& a, const FastFixed<S, K>& b) {
    return a - Fixed<S, K>(static_cast<double>(b));
}

template<size_t S, size_t K>
Fixed<S, K> operator-(const FastFixed<S, K>& a, const Fixed<S, K>& b) {
    return Fixed<S, K>(static_cast<double>(a)) - b;
}

template<size_t S, size_t K>
Fixed<S, K> operator*(const Fixed<S, K>& a, const FastFixed<S, K>& b) {
    return a * Fixed<S, K>(static_cast<double>(b));
}

template<size_t S, size_t K>
Fixed<S, K> operator*(const FastFixed<S, K>& a, const Fixed<S, K>& b) {
    return Fixed<S, K>(static_cast<double>(a)) * b;
}

template<size_t S, size_t K>
Fixed<S, K> operator/(const Fixed<S, K>& a, const FastFixed<S, K>& b) {
    return a / Fixed<S, K>(static_cast<double>(b));
}

template<size_t S, size_t K>
Fixed<S, K> operator/(const FastFixed<S, K>& a, const Fixed<S, K>& b) {
    return Fixed<S, K>(static_cast<double>(a)) / b;
}

template<size_t S, size_t K>
bool operator<(const Fixed<S, K>& a, const FastFixed<S, K>& b) {
    return a < Fixed<S, K>(static_cast<double>(b));
}

template<size_t S, size_t K>
bool operator<(const FastFixed<S, K>& a, const Fixed<S, K>& b) {
    return Fixed<S, K>(static_cast<double>(a)) < b;
}

template<size_t S, size_t K>
bool operator<=(const Fixed<S, K>& a, const FastFixed<S, K>& b) {
    return a <= Fixed<S, K>(static_cast<double>(b));
}

template<size_t S, size_t K>
bool operator<=(const FastFixed<S, K>& a, const Fixed<S, K>& b) {
    return Fixed<S, K>(static_cast<double>(a)) <= b;
}

template<size_t S, size_t K>
bool operator>(const Fixed<S, K>& a, const FastFixed<S, K>& b) {
    return a > Fixed<S, K>(static_cast<double>(b));
}

template<size_t S, size_t K>
bool operator>(const FastFixed<S, K>& a, const Fixed<S, K>& b) {
    return Fixed<S, K>(static_cast<double>(a)) > b;
}

template<size_t S, size_t K>
bool operator>=(const Fixed<S, K>& a, const FastFixed<S, K>& b) {
    return a >= Fixed<S, K>(static_cast<double>(b));
}

template<size_t S, size_t K>
bool operator>=(const FastFixed<S, K>& a, const Fixed<S, K>& b) {
    return Fixed<S, K>(static_cast<double>(a)) >= b;
}

template<size_t S, size_t K>
bool operator==(const Fixed<S, K>& a, const FastFixed<S, K>& b) {
    return a == Fixed<S, K>(static_cast<double>(b));
}

template<size_t S, size_t K>
bool operator==(const FastFixed<S, K>& a, const Fixed<S, K>& b) {
    return Fixed<S, K>(static_cast<double>(a)) == b;
}

template<size_t S, size_t K>
Fixed<S, K>& operator+=(Fixed<S, K>& a, const FastFixed<S, K>& b) {
    a += Fixed<S, K>(static_cast<double>(b));
    return a;
}

template<size_t S, size_t K>
Fixed<S, K>& operator-=(Fixed<S, K>& a, const FastFixed<S, K>& b) {
    a -= Fixed<S, K>(static_cast<double>(b));
    return a;
}

template<size_t S, size_t K>
Fixed<S, K>& operator*=(Fixed<S, K>& a, const FastFixed<S, K>& b) {
    a *= Fixed<S, K>(static_cast<double>(b));
    return a;
}

template<size_t S, size_t K>
Fixed<S, K>& operator/=(Fixed<S, K>& a, const FastFixed<S, K>& b) {
    a /= Fixed<S, K>(static_cast<double>(b));
    return a;
}