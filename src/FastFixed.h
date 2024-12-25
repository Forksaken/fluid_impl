#pragma once

#include "Fixed.h"

template <size_t N, size_t K>
class FastFixed : public Fixed<N, K> {
public:
    FastFixed() : Fixed<N, K>() {}

    FastFixed(int val) : Fixed<N, K>(val) {}

    FastFixed(float f) : Fixed<N, K>(f) {}

    FastFixed(double d) : Fixed<N, K>(d) {}

    FastFixed(const Fixed<N, K>& other) : Fixed<N, K>(other) {}

    FastFixed(const FastFixed<N, K>& other) : Fixed<N, K>(other) {}

    FastFixed& operator=(const Fixed<N, K>& other) {
        Fixed<N, K>::operator=(other);
        return *this;
    }

    FastFixed& operator=(const FastFixed<N, K>& other) {
        Fixed<N, K>::operator=(other);
        return *this;
    }
};
