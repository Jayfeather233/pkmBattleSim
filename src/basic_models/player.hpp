#pragma once

#include "pkm.hpp"
#include "places.hpp"

#include <functional>

class player {
public:
    int badge;
    std::vector<pkm> party_pkm;
    std::vector<pkm> chest_pkm;
    // items
    places *pls;
    int money;
    std::function<std::string()> get_user_input;
    std::function<void(const std::string&)> output2user;

    std::function<bool()> is_op;
    std::function<bool()> is_inner;

    // menu_choose_item
    int menu_choose_pokemon;

    void save(const std::string &filepath);
    static player load(const std::string &filepath);
};