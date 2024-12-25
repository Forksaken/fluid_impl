#pragma once

using namespace std;

#include "config.h"
#include <array>
#include "Fixed.h"
#include "FastFixed.h"
#include "Double.h"
#include "Float.h"
#include "fixed_operators.h"
#include <cassert>
#include <cstring>
#include <limits>
#include <random>
#include <tuple>
#include <vector>

template <typename P, typename V, typename VFLOW, size_t S1, size_t S2>
class Simulator {
public:
    static constexpr array<pair<int, int>, 4> deltas{{{-1, 0}, {1, 0}, {0, -1}, {0, 1}}};
    static constexpr P inf = P::from_raw(numeric_limits<P>::max());
    static constexpr P eps = P::from_raw(numeric_limits<P>::min());

    template <typename T>
    struct VectorFieldStatic {
        array<T, 4> v[S1][S2];

        VectorFieldStatic() {
            for(auto &row : v)
                for(auto &cell : row)
                    cell.fill(T(0));
        }

        T& add(int x, int y, int dx, int dy, T dv) {
            return get(x, y, dx, dy) += dv;
        }

        T& get(int x, int y, int dx, int dy) {
            size_t i = find(deltas.begin(), deltas.end(), pair(dx, dy)) - deltas.begin();
            assert(i < deltas.size());
            return v[x][y][i];
        }
    };

    int N = S1;
    int M = S2;
    int dirs[S1][S2];
    VectorFieldStatic<V> velocity;
    VectorFieldStatic<VFLOW> velocity_flow;
    P rho[256];
    P p[S1][S2];
    P old_p[S1][S2];
    int last_use[S1][S2];
    int UT;
    char field[S1][S2 + 1];

    Simulator() : velocity(), velocity_flow(), UT(0) {
        memset(dirs, 0, sizeof(dirs));
        memset(p, 0, sizeof(p));
        memset(old_p, 0, sizeof(old_p));
        memset(last_use, 0, sizeof(last_use));
        memset(field, 0, sizeof(field));
    }

    struct ParticleParams {
        char type;
        P cur_p;
        array<V, 4> v;

        void swap_with(Simulator &sim, int x, int y) {
            swap(sim.field[x][y], type);
            swap(sim.p[x][y], cur_p);
            swap(sim.velocity.v[x][y], v);
        }
    };

    tuple<P, bool, pair<int, int>> propagate_flow(int x, int y, P lim) {
        last_use[x][y] = UT - 1;
        P ret = 0;
        for(auto &[dx, dy] : deltas) {
            int nx = x + dx, ny = y + dy;
            if(field[nx][ny] != '#' && last_use[nx][ny] < UT) {
                auto cap = velocity.get(x, y, dx, dy);
                auto flow = velocity_flow.get(x, y, dx, dy);
                if(flow == cap)
                    continue;
                auto vp = min(lim, cap - flow);
                if(last_use[nx][ny] == UT - 1) {
                    velocity_flow.add(x, y, dx, dy, vp);
                    last_use[nx][ny] = UT;
                    return {vp, true, {nx, ny}};
                }
                auto [t, prop, end] = propagate_flow(nx, ny, vp);
                ret += t;
                if(prop) {
                    velocity_flow.add(x, y, dx, dy, t);
                    last_use[x][y] = UT;
                    return {t, prop && end != pair(x, y), end};
                }
            }
        }
        last_use[x][y] = UT;
        return {ret, false, {0, 0}};
    }

    void propagate_stop(int x, int y, bool force = false) {
        if(!force) {
            bool stop = true;
            for(auto &[dx, dy] : deltas) {
                int nx = x + dx, ny = y + dy;
                if(field[nx][ny] != '#' && last_use[nx][ny] < UT - 1 && velocity.get(x, y, dx, dy) > 0) {
                    stop = false;
                    break;
                }
            }
            if(!stop)
                return;
        }
        last_use[x][y] = UT;
        for(auto &[dx, dy] : deltas) {
            int nx = x + dx, ny = y + dy;
            if(field[nx][ny] == '#' || last_use[nx][ny] == UT || velocity.get(x, y, dx, dy) > 0)
                continue;
            propagate_stop(nx, ny);
        }
    }

    P move_prob(int x, int y) {
        P sum = 0;
        for(auto &[dx, dy] : deltas) {
            int nx = x + dx, ny = y + dy;
            if(field[nx][ny] == '#' || last_use[nx][ny] == UT)
                continue;
            auto v = velocity.get(x, y, dx, dy);
            if(v < 0)
                continue;
            sum += v;
        }
        return sum;
    }

    bool propagate_move(int x, int y, bool is_first) {
        last_use[x][y] = UT - is_first;
        bool ret = false;
        int nx = -1, ny = -1;
        do {
            array<P, 4> tres;
            P sum = 0;
            for(size_t i = 0; i < deltas.size(); ++i) {
                auto &[dx, dy] = deltas[i];
                int nx = x + dx, ny = y + dy;
                if(field[nx][ny] == '#' || last_use[nx][ny] == UT) {
                    tres[i] = sum;
                    continue;
                }
                auto v = velocity.get(x, y, dx, dy);
                if(v < 0) {
                    tres[i] = sum;
                    continue;
                }
                sum += v;
                tres[i] = sum;
            }

            if(sum == 0)
                break;

            P p_val = (rand() % 1000000) / 1000000.0;
            P p_scaled = sum * p_val;
            size_t d = upper_bound(tres.begin(), tres.end(), p_scaled) - tres.begin();

            auto &[dx, dy] = deltas[d];
            nx = x + dx;
            ny = y + dy;
            assert(velocity.get(x, y, dx, dy) > 0 && field[nx][ny] != '#' && last_use[nx][ny] < UT);

            ret = (last_use[nx][ny] == UT - 1 || propagate_move(nx, ny, false));
        } while(!ret);
        last_use[x][y] = UT;
        for(auto &[dx, dy] : deltas) {
            int nx = x + dx, ny = y + dy;
            if(field[nx][ny] != '#' && last_use[nx][ny] < UT - 1 && velocity.get(x, y, dx, dy) < 0) {
                propagate_stop(nx, ny);
            }
        }
        if(ret && !is_first) {
            ParticleParams pp{};
            pp.swap_with(*this, x, y);
            pp.swap_with(*this, nx, ny);
            pp.swap_with(*this, x, y);
        }
        return ret;
    }

    void runSimulation(size_t T=500, size_t save_interval=0, const string &file_name="") {
        if(rho[' '] == 0 || inf == 0)
            return;

        for(size_t x = 0; x < N; ++x) {
            for(size_t y = 0; y < M; ++y) {
                if(field[x][y] == '#')
                    continue;
                for(auto &[dx, dy] : deltas) {
                    dirs[x][y] += (field[x + dx][y + dy] != '#');
                }
            }
        }

        for(size_t i = 0; i < T; ++i) {
            P total_delta_p = 0;
            for(size_t x = 0; x < N; ++x) {
                for(size_t y = 0; y < M; ++y) {
                    if(field[x][y] == '#')
                        continue;
                    if(field[x + 1][y] != '#')
                        velocity.add(x, y, 1, 0, inf);
                }
            }

            memcpy(old_p, p, sizeof(p));
            for(size_t x = 0; x < N; ++x) {
                for(size_t y = 0; y < M; ++y) {
                    if(field[x][y] == '#')
                        continue;
                    for(auto &[dx, dy] : deltas) {
                        int nx = x + dx, ny = y + dy;
                        if(field[nx][ny] != '#' && old_p[nx][ny] < old_p[x][y]) {
                            auto delta_p = old_p[x][y] - old_p[nx][ny];
                            auto force = delta_p;
                            auto &contr = velocity.get(nx, ny, -dx, -dy);
                            if(force <= contr * rho[(int)field[nx][ny]]) {
                                contr -= force / rho[(int)field[nx][ny]];
                                continue;
                            }
                            force -= contr * rho[(int)field[nx][ny]];
                            contr = 0;
                            velocity.add(x, y, dx, dy, force / rho[(int)field[x][y]]);
                            p[x][y] -= force / dirs[x][y];
                            total_delta_p -= force / dirs[x][y];
                        }
                    }
                }
            }

            velocity_flow = VectorFieldStatic<VFLOW>();
            bool prop = false;
            do {
                UT += 2;
                prop = false;
                for(size_t x = 0; x < N; ++x) {
                    for(size_t y = 0; y < M; ++y) {
                        if(field[x][y] != '#' && last_use[x][y] != UT) {
                            auto [t, local_prop, _] = propagate_flow(x, y, 1);
                            if(t > 0)
                                prop = true;
                        }
                    }
                }
            } while(prop);

            for(size_t x = 0; x < N; ++x) {
                for(size_t y = 0; y < M; ++y) {
                    if(field[x][y] == '#')
                        continue;
                    for(auto &[dx, dy] : deltas) {
                        auto old_v = velocity.get(x, y, dx, dy);
                        auto new_v = velocity_flow.get(x, y, dx, dy);
                        if(old_v > 0) {
                            assert(new_v <= old_v);
                            velocity.get(x, y, dx, dy) = new_v;
                            auto force = (old_v - new_v) * rho[(int)field[x][y]];
                            if(field[x][y] == '.')
                                force *= P(0.8);
                            if(field[x + dx][y + dy] == '#') {
                                p[x][y] += force / dirs[x][y];
                                total_delta_p += force / dirs[x][y];
                            }
                            else {
                                p[x + dx][y + dy] += force / dirs[x + dx][y + dy];
                                total_delta_p += force / dirs[x + dx][y + dy];
                            }
                        }
                    }
                }
            }

            UT += 2;
            prop = false;
            for(size_t x = 0; x < N; ++x) {
                for(size_t y = 0; y < M; ++y) {
                    if(field[x][y] != '#' && last_use[x][y] != UT) {
                        if(move_prob(x, y) > (rand() % 1000000) / 1000000.0) {
                            prop = true;
                            propagate_move(x, y, true);
                        }
                        else {
                            propagate_stop(x, y, true);
                        }
                    }
                }
            }

            if(prop) {
                for(size_t x = 0; x < N; ++x) {
                    for(size_t y = 0; y < M; ++y) {
                        cout << field[x][y];
                    }
                    cout << "\n";
                }
            }
        }
    }
};