#pragma once

#include "pkm.hpp"
#include "places.hpp"

class player {
public:
    int badge;
    pkm party_pkm[6];
    std::vector<pkm> chest_pkm;
    // items
    places *pls;
    int money;

    // menu_choose_item
    int menu_choose_pokemon;
};