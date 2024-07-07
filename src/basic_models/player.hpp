#pragma once

#include "pkm.hpp"
#include "places.hpp"

#include <functional>
#include <set>

class settings {
public:
    std::set<std::string> user_types;
    std::set<std::string> user_enables;

    bool is_type(const std::string &tp) const;

    settings();
    settings(const std::set<std::string> &ut, const std::set<std::string> &ue);
};

class player {
public:
    int badge; // >=0 is a player, =-1 is a wild pkm, =-2 is a trainer
    std::string name;
    std::vector<pkm> party_pkm;
    std::vector<pkm> chest_pkm;
    // items
    places *pls;
    int money;
    settings st;
    std::function<std::string()> get_user_input;
    std::function<void(const std::string &)> output2user;

    std::function<bool()> is_op;
    bool is_type(const std::string &tp) const;

    struct menu_temp{
        // menu_choose_item
        int menu_choose_pokemon;
        int menu_choose_id;
        int menu_choose_position;
        int battle_change_pkm;
    } mt;

    bool sig_save;

    pkm *get_choose_pkm();
    const pkm *get_choose_pkm_const() const;

    player(int bad, std::string name, std::vector<pkm> pp, std::vector<pkm> cp,
           places *pl, int mon, std::function<std::string()> gui,
           std::function<void(const std::string &)> opu,
           std::function<bool()> iop, settings sts = settings());

    void save(const std::string &filepath);
    static player load(const std::string &filepath);

    int get_subsitute_pkm(const pkm* u) const;
};