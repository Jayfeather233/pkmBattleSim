#pragma once

#include "pkm.hpp"
#include "places.hpp"

#include <functional>
#include <set>

class settings {
public:
    std::set<std::string> user_types;
    std::set<std::string> user_enables;

    bool is_type(const std::string &tp)
    {
        return user_types.find(tp) != user_types.end() &&
               user_enables.find(tp) != user_enables.end();
    }

    settings() {}
    settings(const std::set<std::string> &ut, const std::set<std::string> &ue)
        : user_types(ut), user_enables(ue)
    {
    }
};

class player {
public:
    int badge;
    std::vector<pkm> party_pkm;
    std::vector<pkm> chest_pkm;
    // items
    places *pls;
    int money;
    settings st;
    std::function<std::string()> get_user_input;
    std::function<void(const std::string &)> output2user;

    std::function<bool()> is_op;
    bool is_type(const std::string &tp) { return st.is_type(tp); }

    // menu_choose_item
    int menu_choose_pokemon;

    player(int bad, std::vector<pkm> pp, std::vector<pkm> cp, places *pl,
           int mon, std::function<std::string()> gui,
           std::function<void(const std::string &)> opu,
           std::function<bool()> iop, settings sts = settings())
        : badge(bad), party_pkm(pp), chest_pkm(cp), pls(pl), money(mon),
          st(sts), get_user_input(gui), output2user(opu), is_op(iop),
          menu_choose_pokemon(-1)
    {
    }

    void save(const std::string &filepath);
    static player load(const std::string &filepath);
};