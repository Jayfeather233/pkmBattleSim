#pragma once

#include "pkm.hpp"
#include "places.hpp"

#include <functional>
#include <set>

/**
 * this can influence showing addtional options in menu.
 * user_types: op can change what types can user enable
 * user_enables: the user enabled additional function
 */
class settings {
public:
    std::set<std::string> user_types;
    std::set<std::string> user_enables;

    bool is_type(const std::string &tp) const;

    settings();
    settings(const std::set<std::string> &ut, const std::set<std::string> &ue);
};

class battle_main;

class player {
public:
    int badge; // >=0 is a player, =-1 is a wild pkm, =-2 is a trainer
    std::string name;
    std::vector<pkm> party_pkm;
    std::vector<pkm> chest_pkm;
    std::vector<std::pair<int, int>> items; // item_id - item amount
    places *pls;
    places *las_store_place;
    int money;
    settings st;
    std::function<std::string()> get_user_input;
    std::function<std::string()> get_user_input_no_wait;
    std::function<void(const std::string &)> output2user;

    std::function<bool()> is_op;
    bool is_type(const std::string &tp) const; // used for showing additional menu

    struct menu_temp {
        int menu_choose_item;
        int menu_choose_pokemon;
        int menu_choose_id;
        int menu_choose_position;
        int battle_change_pkm;
        int move_point;

        bool is_goback;
    } mt;

    bool sig_save; // anything changes that need to save to file

    battle_main *bm;

    pkm *get_choose_pkm();
    const pkm *get_choose_pkm_const() const;

    player(int bad, std::string name, std::vector<pkm> pp, std::vector<pkm> cp, places *pl, places *laspl, int mon,
           std::function<std::string(bool)> gui, std::function<void(const std::string &)> opu,
           std::function<bool()> iop, settings sts = settings());

    void save(const std::string &filepath);
    player(const std::string &filepath);
    player(const pkm &p);                                       // a wild pkm
    player(const std::string &name, const std::vector<pkm> &p); // a trainer

    void gain_exp(size_t position, size_t exp);

    int get_subsitute_pkm(const std::vector<pkm *> u);
    std::vector<pkm *> get_available_pkm() const;
};