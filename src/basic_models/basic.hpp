#pragma once

#define MAX_pts 255

#include <algorithm>
#include <string>

struct base6 {
    // health point, attak, special attack,
    // defence, special defence, speed
    int hp, atk, stk, df, sdf, spd;
};

base6 operator+(const base6 &a, const base6 &b);

base6 operator+=(base6 &a, const base6 &b);

enum element_types { EMPTY=0, PLAIN, FIRE, WATER, GRASS };

// from, to, types Compatibility
extern float eff_table[20][20];
extern float eff_table[20][20];

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

enum gender {
    MALE,
    FEMALE,
    NONE
};

void init_predefs();