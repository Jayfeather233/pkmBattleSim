#pragma once

#define MAX_pts 255

#include <algorithm>
#include <string>

struct base6 {
    // health point, attak, special attack,
    // defence, special defence, speed
    int hp, atk, stk, df, sdf, spd;
};

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

enum element_types { EMPTY, PLAIN, FIRE, WATER, GRASS };

// from, to, types Compatibility
float eff_table[20][20] = {{1.0, 1.0, 1.0, 1.0, 1.0},
                           {1.0, 1.0, 1.0, 1.0, 1.0},
                           {1.0, 1.0, 0.5, 0.5, 2.0},
                           {1.0, 1.0, 2.0, 0.5, 0.5},
                           {1.0, 1.0, 0.5, 2.0, 0.5}};

enum battle_status {
    NORMAL,
    SLEEP,
    ELECT,
    POISON,
};

enum battle_position {
    SELF,  // self
    OPPO,  // the one opposite
    OURS,  // other pkm on our side
    THEIR, // other pkm on opposite
    AROUND // pkm around self
};
