#pragma once

#define MAX_pts 255

#include <algorithm>
#include <string>

struct base6 {
    // health point, attak, special attack,
    // defence, special defence, speed
    int hp, atk, df, stk, sdf, spd;
};

base6 operator+(const base6 &a, const base6 &b);

base6 operator+=(base6 &a, const base6 &b);

enum element_types {
    EMPTY = 0,
    PLAIN,
    FIRE,
    WATER,
    GRASS,
    POISON,
    FIGHT,
    FLY,
    ELEC,
    GROUND,
    PSYCHIC,
    ROCK,
    ICE,
    BUG,
    DRAGON,
    GHOST,
    DARK,
    STEEL,
    FAIRY
};
element_types str2et(const std::string &s);

// from, to, types Compatibility
extern float eff_table[20][20];

enum battle_status {
    NORMAL,
    SLEEP,
    ELECT,
    POISONED,
};

enum move_target {
    SPECIFIC, // 镜面反射
    ANY_OPPO,
    ANY_ALLY,
    USER_FIELD,
    UESR_OR_AROUND_ALLY,
    OPPO_FIELD,
    USER,
    RANDOM_OPPO,
    ALL_OPPO,
    SELECTED_OPPO,
    ENTIRE_FIELD,
    USER_AND_ALL_ALLY,
    ALL_ALLY,
    ALL,
    ALL_FAINT
};

enum gender { MALE, FEMALE, NONE };

void init_predefs();

enum field_status{
    NO_FIELD,
    TRICK_ROOM //戏法空间
};
enum weather_status{
    CLEAR
};