#pragma once

/*
    For the basic `turn`(回合), please refer to:
    https://wiki.52poke.com/wiki/%E5%9B%9E%E5%90%88
*/

#include "menu.hpp"

struct move_struct {
    pkm *from;
    int from_pos;
    int from_side;
    int skill_id;     // skills_list[from->skills[skill_id]]
    int to;           // selected one
    int mv_piror_fix; // like: 先制之爪+1，后攻之尾-1
    // use item

    int use_item;
    int exchange_pkm; // exchange to player.party_pkm[exchange_pkm]
};

bool ms_cmp_pkm_slow2fast(const move_struct &a, const move_struct &b);
bool ms_cmp_pkm_fast2slow(const move_struct &a, const move_struct &b);
bool ms_cmp_move_slow2fast(const move_struct &a, const move_struct &b);
bool ms_cmp_move_fast2slow(const move_struct &a, const move_struct &b);

// for pkm exchange
struct pkm_list_item {
    pkm *from;
    int from_side;
    size_t from_pos;
    pkm *to;
};

inline bool pli_cmp_slow2fast(const pkm_list_item &a, const pkm_list_item &b)
{
    return a.from->stat.spd < b.from->stat.spd;
}
inline bool pli_cmp_fast2slow(const pkm_list_item &a, const pkm_list_item &b) { return !(pli_cmp_slow2fast(a, b)); }

class battle_main {
public:
    size_t battle_num;
    bool is_escape;
    unsigned int escape_times;
    std::array<player *, 2> p;
    std::array<std::vector<pkm *>, 2> pkms;
    std::array<std::vector<std::set<pkm *>>, 2> seened_pkms;
    std::vector<move_struct> moves; // moves in one turn

    std::array<field_status, 3> field; // 0/1 player field, 2 overall field
    weather_status weather;

    battle_main(size_t bnum, std::array<player *, 2> px, std::array<std::vector<pkm *>, 2> pkmsx, weather_status ws);

    enum sort_method { BY_PKM_SPEED, BY_MOVES };

    std::string half_to_string(int side);

    std::string to_string(int side);

    void sort_moves(sort_method method);

    inline bool check_valid_pos(int side, int id) { return side < 2 && 0 <= side && 0 <= id && static_cast<size_t>(id) < pkms[side].size(); }
    inline bool check_valid_pkm(int side, int id) { return check_valid_pos(side, id) && pkms[side][id] != nullptr; }
    inline bool check_valid_pos(int side, size_t id) { return side < 2 && id < pkms[side].size(); }
    inline bool check_valid_pkm(int side, size_t id) { return check_valid_pos(side, id) && pkms[side][id] != nullptr; }

    std::vector<std::pair<int, int>> aff_pkms(const move_struct &ms);

    void set_prior_fix();

    void refresh_seened_pkm();
    void output_seend_pkm() const;
    void output_pkm_status() const;

    /**
     * If the pkm[side][pos] is faint, get the exp gained
     */
    void get_gained_exp(int side, int pos);

    void pkm_entry_proc(int side, int id, pkm *pkm, bool is_first_entry = false);
    void execute_move(const move_struct &ms);

    void field_effect(field_status fs, pkm *p);
    void execute_change_pkm(const pkm_list_item &ms);
    void exchange_pkm(bool first_turn);
    void use_items();
    void use_moves();
    void end_turn_proc();
    int check_status();
    bool escape_detec();
    int process_turn(bool first_turn);
};

extern text_menu *battle_menu;

void get_next_battle_move(int side, battle_main *bm);

// battle_num: n vs n
int battle_start(player *p1, player *p2, size_t battle_num, weather_status weather = weather_status::CLEAR);