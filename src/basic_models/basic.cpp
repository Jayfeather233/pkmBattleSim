#include "basic.hpp"

float eff_table[20][20] = {{1.0, 1.0, 1.0, 1.0, 1.0},
                           {1.0, 1.0, 1.0, 1.0, 1.0},
                           {1.0, 1.0, 0.5, 0.5, 2.0},
                           {1.0, 1.0, 2.0, 0.5, 0.5},
                           {1.0, 1.0, 0.5, 2.0, 0.5}};

base6 operator+(const base6 &a, const base6 &b)
{
    base6 w = (base6){std::min(MAX_pts, (int)a.hp + b.hp),
                      std::min(MAX_pts, (int)a.atk + b.atk),
                      std::min(MAX_pts, (int)a.stk + b.stk),
                      std::min(MAX_pts, (int)a.df + b.df),
                      std::min(MAX_pts, (int)a.sdf + b.sdf),
                      std::min(MAX_pts, (int)a.spd + b.spd)};
    return w;
}

base6 operator+=(base6 &a, const base6 &b)
{
    a = (base6){std::min(MAX_pts, (int)a.hp + b.hp),
                std::min(MAX_pts, (int)a.atk + b.atk),
                std::min(MAX_pts, (int)a.stk + b.stk),
                std::min(MAX_pts, (int)a.df + b.df),
                std::min(MAX_pts, (int)a.sdf + b.sdf),
                std::min(MAX_pts, (int)a.spd + b.spd)};
    return a;
}