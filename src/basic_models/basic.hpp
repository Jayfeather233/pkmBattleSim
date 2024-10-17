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
base6 operator-(const base6 &a, const base6 &b);
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
extern base6 nature_table6[25];

enum battle_status {
    NORMAL,
    SLEEP,
    ELECT,
    POISONED,
    BADLY_POISONED,
};

enum move_target {
    AROUND_USER,       // 单体选择
    USER,              // 自身
    ALL_OPPO,          // 任意对手（范围内）
    ANY_OPPO,          // 任一对手（范围内）
    ENTIRE_FIELD,      // weather
    ALL_AROUND_USER,   // except user
    USER_AND_ALL_ALLY, // like: 芳香治疗
    ANY,
    OPPO_FIELD,
    SPECIFIC, // 镜面反射等最后一次伤害
    ANY_ALLY, // 范围内
    ALL_ALLY, // except user
    USER_OR_AROUND_ALLY,
    RANDOM_OPPO,
    USER_FIELD // like: 光墙
};

bool is_no_target_skill(move_target);
bool is_1v1_hit_oppo_skill(move_target);

enum gender { MALE, FEMALE, NONE };

void init_predefs();
void remove_predefs();

enum field_status {
    NO_FIELD,
    TRICK_ROOM,     // 戏法空间
    STICKY_WEB,     // 黏黏网
    SPIKES,         // 撒菱
    SPIKES2,        // 撒菱x2
    SPIKES3,        // 撒菱x3
    TOXIC_SPICKES,  // 毒菱
    TOXIC_SPICKES2, // 毒菱x2
    STEALTH_ROCK    // 隐形岩
};
enum weather_status { CLEAR };