#pragma once

#include "simulator.h"

template <typename P, typename V, typename VFLOW>
class Simulator<P, V, VFLOW, 0, 0> {
public:
    static constexpr array<pair<int, int>, 4> deltas{{{-1, 0}, {1, 0}, {0, -1}, {0, 1}}};
    static constexpr P inf = P::get_infinity();
    static constexpr P eps = P::get_epsilon();

    template <typename T>
    struct VectorFieldDynamic {
        array<T, 4>** v;
        size_t x, y;

        VectorFieldDynamic(size_t x_, size_t y_) : x(x_), y(y_) {
            v = new array<T, 4>*[x];
            for (size_t i = 0; i < x; i++) {
                v[i] = new array<T, 4>[y];
            }
        }

        ~VectorFieldDynamic() {
            for (size_t i = 0; i < x; i++) {
                delete[] v[i];
            }
            delete[] v;
        }

        T& add(int x_, int y_, int dx, int dy, T dv) {
            return get(x_, y_, dx, dy) += dv;
        }

        T& get(int x_, int y_, int dx, int dy) {
            size_t i = find(deltas.begin(), deltas.end(), pair(dx, dy)) - deltas.begin();
            assert(i < deltas.size());
            return v[x_][y_][i];
        }
    };

    size_t N;
    size_t M;
    int** dirs;
    VectorFieldDynamic<V> velocity;
    VectorFieldDynamic<VFLOW> velocity_flow;
    P rho[256];
    P** p, ** old_p;
    int** last_use;
    int UT;
    char** field;

    Simulator(size_t x, size_t y)
        : velocity(x, y), velocity_flow(x, y), N(x), M(y), UT(0) {
        dirs = new int*[x];
        p = new P*[x];
        old_p = new P*[x];
        last_use = new int*[x];
        field = new char*[x];
        for (size_t i = 0; i < x; i++) {
            dirs[i] = new int[y];
            p[i] = new P[y];
            old_p[i] = new P[y];
            last_use[i] = new int[y];
            field[i] = new char[y + 1];
            memset(dirs[i], 0, sizeof(int) * y);
            memset(p[i], 0, sizeof(P) * y);
            memset(old_p[i], 0, sizeof(P) * y);
            memset(last_use[i], 0, sizeof(int) * y);
            memset(field[i], 0, sizeof(char) * (y + 1));
        }
    }

    ~Simulator() {
        for (size_t i = 0; i < N; i++) {
            delete[] dirs[i];
            delete[] p[i];
            delete[] old_p[i];
            delete[] last_use[i];
            delete[] field[i];
        }
        delete[] dirs;
        delete[] p;
        delete[] old_p;
        delete[] last_use;
        delete[] field;
    }
};