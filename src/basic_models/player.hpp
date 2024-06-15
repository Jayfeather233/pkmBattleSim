#pragma once

#include "pkm.hpp"
#include "places.hpp"

class player {
public:
    int badge;
    std::vector<pkm> party_pkm;
    std::vector<pkm> chest_pkm;
    // items
    places *pls;
    int money;

    // menu_choose_item
    int menu_choose_pokemon;

    void save(const std::string &filepath);
    static player load(const std::string &filepath);
};