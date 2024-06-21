#include "basic.hpp"

#include <iostream>

float eff_table[20][20] = {
    // EXMPTY PLAIN  FIRE  WATER  GRASS  POISON  FIGHT  FLY   ELEC  GROUND  PSYCHIC  ROCK  ICE   BUG   DRAGON  GHOST  DARK  STEEL  FAIRY
    {1.0,     1.0,   1.0,  1.0,   1.0,   1.0,    1.0,   1.0,  1.0,  1.0,    1.0,     1.0,  1.0,  1.0,  1.0,    1.0,   1.0,  1.0,   1.0},  // empty
    {1.0,     1.0,   1.0,  1.0,   1.0,   1.0,    1.0,   1.0,  1.0,  1.0,    1.0,     0.5,  1.0,  1.0,  1.0,    0.0,   1.0,  0.5,   1.0},  // PLAIN
    {1.0,     1.0,   0.5,  0.5,   2.0,   1.0,    1.0,   1.0,  1.0,  1.0,    1.0,     0.5,  2.0,  2.0,  0.5,    1.0,   1.0,  2.0,   1.0},  // FIRE
    {1.0,     1.0,   2.0,  0.5,   0.5,   1.0,    1.0,   1.0,  1.0,  2.0,    1.0,     2.0,  1.0,  1.0,  0.5,    1.0,   1.0,  1.0,   1.0},  // WATER
    {1.0,     1.0,   0.5,  2.0,   0.5,   0.5,    1.0,   0.5,  1.0,  2.0,    1.0,     2.0,  1.0,  0.5,  0.5,    1.0,   1.0,  0.5,   1.0},  // GRASS
    {1.0,     1.0,   1.0,  1.0,   2.0,   0.5,    1.0,   1.0,  1.0,  0.5,    1.0,     0.5,  1.0,  1.0,  1.0,    0.5,   1.0,  0.0,   2.0},  // POISON
    {1.0,     2.0,   1.0,  1.0,   1.0,   0.5,    1.0,   0.5,  1.0,  1.0,    0.5,     2.0,  2.0,  0.5,  1.0,    0.0,   2.0,  2.0,   0.5},  // FIGHT
    {1.0,     1.0,   1.0,  1.0,   2.0,   1.0,    2.0,   1.0,  0.5,  1.0,    1.0,     0.5,  1.0,  2.0,  1.0,    1.0,   1.0,  0.5,   1.0},  // FLY
    {1.0,     1.0,   1.0,  2.0,   0.5,   1.0,    1.0,   2.0,  0.5,  0.0,    1.0,     1.0,  1.0,  1.0,  0.5,    1.0,   1.0,  1.0,   1.0},  // ELEC
    {1.0,     1.0,   2.0,  1.0,   0.5,   2.0,    1.0,   0.0,  2.0,  1.0,    1.0,     2.0,  1.0,  0.5,  1.0,    1.0,   1.0,  2.0,   1.0},  // GROUND
    {1.0,     1.0,   1.0,  1.0,   1.0,   2.0,    2.0,   1.0,  1.0,  1.0,    0.5,     1.0,  1.0,  1.0,  1.0,    1.0,   0.0,  0.5,   1.0},  // PSYCHIC
    {1.0,     1.0,   2.0,  1.0,   1.0,   1.0,    0.5,   2.0,  1.0,  0.5,    1.0,     1.0,  2.0,  2.0,  1.0,    1.0,   1.0,  0.5,   1.0},  // ROCK
    {1.0,     1.0,   0.5,  0.5,   2.0,   1.0,    1.0,   2.0,  1.0,  2.0,    1.0,     1.0,  0.5,  1.0,  2.0,    1.0,   1.0,  0.5,   1.0},  // ICE
    {1.0,     1.0,   0.5,  1.0,   2.0,   0.5,    0.5,   0.5,  1.0,  1.0,    2.0,     1.0,  1.0,  1.0,  1.0,    0.5,   2.0,  0.5,   0.5},  // BUG
    {1.0,     1.0,   1.0,  1.0,   1.0,   1.0,    1.0,   1.0,  1.0,  1.0,    1.0,     1.0,  1.0,  1.0,  2.0,    1.0,   1.0,  0.5,   0.0},  // DRAGON
    {1.0,     0.0,   1.0,  1.0,   1.0,   1.0,    1.0,   1.0,  1.0,  1.0,    2.0,     1.0,  1.0,  1.0,  1.0,    2.0,   0.5,  1.0,   1.0},  // GHOST
    {1.0,     1.0,   1.0,  1.0,   1.0,   1.0,    0.5,   1.0,  1.0,  1.0,    2.0,     1.0,  1.0,  1.0,  1.0,    2.0,   0.5,  1.0,   0.5},  // DARK
    {1.0,     1.0,   0.5,  0.5,   1.0,   1.0,    1.0,   1.0,  0.5,  1.0,    1.0,     2.0,  2.0,  1.0,  1.0,    1.0,   1.0,  0.5,   2.0},  // STEEL
    {1.0,     1.0,   0.5,  1.0,   1.0,   0.5,    2.0,   1.0,  1.0,  1.0,    1.0,     1.0,  1.0,  1.0,  1.0,    2.0,   2.0,  0.5,   1.0}   // FAIRY
};

base6 operator+(const base6 &a, const base6 &b)
{
    base6 w = (base6){std::min(MAX_pts, (int)a.hp + b.hp),
                      std::min(MAX_pts, (int)a.atk + b.atk),
                      std::min(MAX_pts, (int)a.df + b.df),
                      std::min(MAX_pts, (int)a.stk + b.stk),
                      std::min(MAX_pts, (int)a.sdf + b.sdf),
                      std::min(MAX_pts, (int)a.spd + b.spd)};
    return w;
}

base6 operator+=(base6 &a, const base6 &b)
{
    a = (base6){std::min(MAX_pts, (int)a.hp + b.hp),
                std::min(MAX_pts, (int)a.atk + b.atk),
                std::min(MAX_pts, (int)a.df + b.df),
                std::min(MAX_pts, (int)a.stk + b.stk),
                std::min(MAX_pts, (int)a.sdf + b.sdf),
                std::min(MAX_pts, (int)a.spd + b.spd)};
    return a;
}

element_types str2et(const std::string &s) {
    std::string lowerCaseS = s;
    std::transform(lowerCaseS.begin(), lowerCaseS.end(), lowerCaseS.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    if (lowerCaseS == "empty" || lowerCaseS == "em") {
        return element_types::EMPTY;
    } else if (lowerCaseS == "plain" || lowerCaseS == "pl") {
        return element_types::PLAIN;
    } else if (lowerCaseS == "fire") {
        return element_types::FIRE;
    } else if (lowerCaseS == "water" || lowerCaseS == "wa") {
        return element_types::WATER;
    } else if (lowerCaseS == "grass" || lowerCaseS == "gr") {
        return element_types::GRASS;
    } else if (lowerCaseS == "poison" || lowerCaseS == "po") {
        return element_types::POISON;
    } else if (lowerCaseS == "fight") {
        return element_types::FIGHT;
    } else if (lowerCaseS == "fly" || lowerCaseS == "fl") {
        return element_types::FLY;
    } else if (lowerCaseS == "elec" || lowerCaseS == "el") {
        return element_types::ELEC;
    } else if (lowerCaseS == "ground" || lowerCaseS == "gr") {
        return element_types::GROUND;
    } else if (lowerCaseS == "psychic" || lowerCaseS == "ps") {
        return element_types::PSYCHIC;
    } else if (lowerCaseS == "rock" || lowerCaseS == "ro") {
        return element_types::ROCK;
    } else if (lowerCaseS == "ice" || lowerCaseS == "ic") {
        return element_types::ICE;
    } else if (lowerCaseS == "bug" || lowerCaseS == "bu") {
        return element_types::BUG;
    } else if (lowerCaseS == "dragon" || lowerCaseS == "dr") {
        return element_types::DRAGON;
    } else if (lowerCaseS == "ghost" || lowerCaseS == "gh") {
        return element_types::GHOST;
    } else if (lowerCaseS == "dark" || lowerCaseS == "da") {
        return element_types::DARK;
    } else if (lowerCaseS == "steel" || lowerCaseS == "st") {
        return element_types::STEEL;
    } else if (lowerCaseS == "fairy" || lowerCaseS == "fa") {
        return element_types::FAIRY;
    } else {
        std::cout<<"[WARNING] unrecognizable type: "<<lowerCaseS<<std::endl;
        return element_types::EMPTY; // Default case if no match is found
    }
}