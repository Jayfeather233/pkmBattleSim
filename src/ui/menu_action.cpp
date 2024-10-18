#include "myutils.hpp"

#include "battle.hpp"
#include "skills.hpp"
#include "menu.hpp"
#include "texts.hpp"
#include <fmt/core.h>

std::string pkm_choosing_name(const pkm &p, bool is_mine)
{
    return fmt::format("{} {} {}", get_side_text(is_mine), p.get_name(), gender2string(p.gend));
}
std::string pkm_choosing_name(const pkm *p, bool is_mine)
{
    return fmt::format("{} {} {}", get_side_text(is_mine), p->get_name(), gender2string(p->gend));
}

std::map<std::string, std::function<void(player &, int)>> choose_callback_mapper = {
    {"menu_choose_pokemon_chest_pkm", [](player &p, int id) { p.mt.menu_choose_pokemon = id + 6; }},
    {"set_choose_pkm", [](player &p, int id) { p.mt.menu_choose_pokemon = id; }},
    {"set_choose_id", [](player &p, int id) { p.mt.menu_choose_id = id; }},
    {"set_choose_item", [](player &p, int id) { p.mt.menu_choose_item = id; }},
    {"set_choose_position", [](player &p, int id) { p.mt.menu_choose_position = id; }},
    {"set_battle_change", [](player &p, int id) { p.mt.battle_change_pkm = id; }}};

std::map<std::string, std::function<void(player &)>> action_mapper = {
    {"output_pkm_state",
     [](player &p) {
         const pkm *u = p.get_choose_pkm_const();
         std::string s1 =
             fmt::format("{} 的性格\n {} 在 {} 遇到了它。", nature2str[u->nature], u->get_time, u->get_place);
         std::string s2 = fmt::format("{} Lv. {}, {}, 持有物：{}", u->get_name(), u->level, gender2string(u->gend),
                                      u->carried_item); // TOOD: item here
         std::string s3 =
             fmt::format("No. {}, type: {}, 现有经验值: {}, 距下一级经验值: {}", u->id, eletype2string(u->typ),
                         u->exp_curr, get_next_level_exp(u->exp_acc_speed, u->level) - u->exp_curr);
         p.output2user(fmt::format("{}\n\n{}\n\n{}", s1, s2, s3));
     }},
    {"output_place_point",
     [](player &p) {
         p.output2user(fmt::format("距去往下一地点还需：{} 点数", p.pls->meet_points - p.mt.move_point));
     }},
    {"output_place_desc", [](player &p) { p.output2user(fmt::format("{}\n{}", p.pls->name, p.pls->desc)); }},
    {"reset_menu_choose_pokemon", [](player &p) { p.mt.menu_choose_pokemon = -1; }},
    {"refresh_choosed_pkm",
     [](player &p) {
         p.get_choose_pkm()->bstatus = battle_status::NORMAL;
         p.output2user(get_action_text("refresh", p));
     }},
    {"reset_all_menu_tmp",
     [](player &p) {
         p.mt.menu_choose_item = -1;
         p.mt.battle_change_pkm = -1;
         p.mt.menu_choose_id = -1;
         p.mt.menu_choose_pokemon = -1;
         p.mt.menu_choose_position = -1;
         p.mt.is_goback = false;
     }},
    {"set_default_choose_position", [](player &p) { p.mt.menu_choose_position = 0; }},
    {"battle_wild_pkm",
     [](player &p) {
         auto cmp = [](const pkm &a, const pkm &b) { return a.level < b.level; };
         pkm u = pkm::create_pkm(
             pkm_list[get_possi_random(p.pls->pkms)], "",
             get_random(7) - 5 + std::max_element(p.party_pkm.begin(), p.party_pkm.end(), cmp)->level, "", p.pls->name);
         player *wp = new player(u);
         int result = battle_start(&p, wp, 1);
         delete wp;
         p.sig_save = true;
         p.mt.move_point += 1;
         if (p.mt.move_point >= p.pls->meet_points)
             p.st.user_enables.insert("enough_place_point");
         if (p.all_pkm_faint()){
            // TODO
         }
     }},
    {"move_to_next_place",
     [](player &p) {
         if (p.mt.move_point >= p.pls->meet_points) {
             p.mt.move_point = 0;
             p.pls = p.pls->neighbors[p.mt.menu_choose_id];
             if (p.mt.move_point >= p.pls->meet_points)
                 p.st.user_enables.insert("enough_place_point");
             else
                 p.st.user_enables.erase("enough_place_point");
             p.output2user(get_action_text("move_to_next_place", p));
             p.sig_save = true;
         }
         else {
             // TODO: text here
         }
     }},
    {"set_is_goback_true", [](player &p) { p.mt.is_goback = true; }},
    {"battle_target_choose", [](player &p) {
         base_skill *bs = skill_list[p.party_pkm[p.mt.menu_choose_pokemon].skills[p.mt.menu_choose_id]];
         if ((p.bm->battle_num == 1 && is_1v1_hit_oppo_skill(bs->tgt)) || is_no_target_skill(bs->tgt)) {
             p.mt.menu_choose_position = 0;
         }
         else {
             run_text_menu(p, battle_target_choose_menu, nullptr, root_menu);
         }
     }},
     {"restore_all_pkm", [](player &p) {
        for(pkm &u : p.party_pkm){
            u.hpreduced = 0;
            for(size_t i=0;i<4;++i){
                u.used_pp[i] = 0;
            }
        }
        for(pkm &u : p.chest_pkm){
            u.hpreduced = 0;
            for(size_t i=0;i<4;++i){
                u.used_pp[i] = 0;
            }
        }
        p.las_store_place = p.pls;
     }},
     {"player_buy_item", [](player &p){
        auto item = map_finder((std::string)"get_mart_sell_items", get_choose_set_mapper)(p)[p.mt.menu_choose_id];
        // TODO
     }}
     
     };

std::map<std::string, std::function<std::vector<std::string>(const player &)>> get_choose_set_mapper = {
    {"get_player_party_pkm_name_list",
     [](const player &p) -> std::vector<std::string> {
         std::vector<std::string> ret;
         for (auto pk : p.party_pkm) {
             if (pk.id != 0) {
                 ret.push_back(pk.get_name());
             }
         }
         return ret;
     }},
    {"get_player_chest_pkm_name_list",
     [](const player &p) -> std::vector<std::string> {
         std::vector<std::string> ret;
         for (auto it : p.chest_pkm) {
             if (it.id != 0) {
                 ret.push_back(it.get_name());
             }
         }
         return ret;
     }},
    {"get_player_available_pkm_name_list",
     [](const player &p) -> std::vector<std::string> {
         std::vector<std::string> ret;
         auto s = p.get_available_pkm();
         for (auto it : s) {
             ret.push_back(it->get_name());
         }
         return ret;
     }},
    {"get_place_neighber_name_list",
     [](const player &p) -> std::vector<std::string> {
         std::vector<std::string> ret;
         for (auto it : p.pls->neighbors) {
             ret.push_back(it->name);
         }
         return ret;
     }},
    {"get_pkm_skill_list",
     [](const player &p) -> std::vector<std::string> {
         std::vector<std::string> ret;
         for (auto it : p.party_pkm[p.mt.menu_choose_pokemon].skills) {
             ret.push_back(skill_list[it]->name);
         }
         return ret;
     }},
    {"get_pkm_skill_target_list", [](const player &p) -> std::vector<std::string> {
         std::vector<std::string> ret;
         base_skill *bs = skill_list[p.party_pkm[p.mt.menu_choose_pokemon].skills[p.mt.menu_choose_id]];
         if (is_no_target_skill(bs->tgt)) {
             ret.push_back("hi, bugs here XD, you cannot select specific "
                           "target for this move");
         }
         else {
             int side = (&p == p.bm->p[1] ? 1 : 0);
             int position;
             for (position = 0; position < p.bm->pkms[side].size() &&
                                p.bm->pkms[side][position] != &p.party_pkm[p.mt.menu_choose_pokemon];
                  ++position)
                 ;
             if (bs->tgt == move_target::AROUND_USER) {
                 if (p.bm->check_valid_pos(side, position - 1)) {
                     if (p.bm->check_valid_pkm(side, position - 1)) {
                         ret.push_back(pkm_choosing_name(p.bm->pkms[side][position - 1], true));
                     }
                     else {
                         ret.push_back(get_side_text(true) + " empty");
                     }
                 }
                 for (int i = position - 1; i <= position + 1; i++) {
                     if (p.bm->check_valid_pos(side ^ 1, i)) {
                         if (p.bm->check_valid_pkm(side ^ 1, i)) {
                             ret.push_back(pkm_choosing_name(p.bm->pkms[side ^ 1][i], false));
                         }
                         else {
                             ret.push_back(get_side_text(false) + " empty");
                         }
                     }
                 }
                 if (p.bm->check_valid_pos(side, position + 1)) {
                     if (p.bm->check_valid_pkm(side, position + 1)) {
                         ret.push_back(pkm_choosing_name(p.bm->pkms[side][position + 1], true));
                     }
                     else {
                         ret.push_back(get_side_text(true) + " empty");
                     }
                 }
             }
             else if (bs->tgt == move_target::ANY_OPPO) {
                 for (int i = -1; i <= 1; ++i) {
                     if (p.bm->check_valid_pos(side ^ 1, i + position)) {
                         if (p.bm->check_valid_pkm(side ^ 1, i + position)) {
                             ret.push_back(pkm_choosing_name(p.bm->pkms[side ^ 1][i + position], false));
                         }
                         else {
                             ret.push_back(get_side_text(false) + " empty");
                         }
                     }
                 }
             }
             else if (bs->tgt == move_target::ANY) {
                 for (size_t i = 0; i < p.bm->pkms[side ^ 1].size(); ++i) {
                     if (p.bm->check_valid_pkm(side ^ 1, i)) {
                         ret.push_back(pkm_choosing_name(p.bm->pkms[side ^ 1][i], false));
                     }
                     else {
                         ret.push_back(get_side_text(false) + " empty");
                     }
                 }
                 for (size_t i = 0; i < p.bm->pkms[side].size(); ++i) {
                     if (p.bm->check_valid_pkm(side, i)) {
                         ret.push_back(pkm_choosing_name(p.bm->pkms[side][i], true));
                     }
                     else {
                         ret.push_back(get_side_text(true) + "empty");
                     }
                 }
             }
             else if (bs->tgt == move_target::ANY_ALLY) {
                 for (int i = -1; i <= 1; ++i) {
                     if (i == 0)
                         continue;
                     if (p.bm->check_valid_pos(side, i + position)) {
                         if (p.bm->check_valid_pkm(side, i + position)) {
                             ret.push_back(pkm_choosing_name(p.bm->pkms[side][i + position], true));
                         }
                         else {
                             ret.push_back(get_side_text(true) + "empty");
                         }
                     }
                 }
             }
             else if (bs->tgt == move_target::USER_OR_AROUND_ALLY) {
                 for (int i = -1; i <= 1; ++i) {
                     if (p.bm->check_valid_pos(side, i + position)) {
                         if (p.bm->check_valid_pkm(side, i + position)) {
                             ret.push_back(pkm_choosing_name(p.bm->pkms[side][i + position], true));
                         }
                         else {
                             ret.push_back(get_side_text(true) + "empty");
                         }
                     }
                 }
             }
         }
         return ret;
     }},
     {"get_mart_sell_items", [](const player &p) -> std::vector<std::string> {
        // TODO: 
     }}};