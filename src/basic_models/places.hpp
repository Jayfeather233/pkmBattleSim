#pragma once

#include <string>
#include <vector>

#include "pkm.hpp"

// like alolan
class region {
public:
    std::string name;
    std::string desc;
};

// like the 1st road
class places {
public:
    std::string name;
    std::string desc;
    // belong to what region
    region *regi;
    int meet_points; // wild pokemon 1pts, trainer 2pts, meet the points
                     // required to continue to next place
    // can go to where
    std::vector<places *> neighbors;
    // what pkm can come across, <pkm_id, chances>
    std::vector<std::pair<int, float>> pkms;
    int id;
};

extern std::vector<region> regis;
extern std::vector<places> placs;