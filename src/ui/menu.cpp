#include "menu.hpp"
#include "battle.hpp"
#include "myutils.hpp"
#include "random.hpp"
#include "texts.hpp"
#include "utils.h"
#include <fmt/core.h>

void run_text_menu(player &p, const text_menu *px, std::function<void()> save,
                   const text_menu *stop)
{
    while (px != stop && px != nullptr) {
        while (px != stop && px != nullptr && px->no_next_menu(p)) {
            if (px->father != nullptr && px->father->action != nullptr) {
                px->father->action(p);
            }
            px = px->father;
        }
        if (px == stop || px == nullptr) {
            break;
        }
        p.output2user(px->to_string(p));
        px = px->get_nxt_menu(p, get_next_int(p));
        if (p.sig_save) {
            p.sig_save = false;
            save();
        }
    }
}

std::map<std::string, std::map<std::string, std::vector<text_menu *>>>
    app_menu_mapper;

std::string text_menu::get_title(const player &p) const
{
    std::string s = this->title;
    my_string_format(s, p);
    return s;
}

bool text_menu::no_next_menu(const player &p) const
{
    if (is_choose) {
        return false;
    }
    else {
        if (options.size())
            return false;
        for (auto it : app_options) {
            if (p.is_type(it.first)) {
                if (it.second.size())
                    return false;
            }
        }
    }
    return true;
}

std::string name_pkm(player &p)
{
    p.output2user(fmt::format("想给它起名字吗？\n0. 是\n1. 否"));
    if (p.get_user_input() == "0") {
        p.output2user(fmt::format("请输入名字："));
        return p.get_user_input();
    }
    else {
        return "";
    }
}

text_menu::text_menu() { father = nullptr; }

text_menu::text_menu(
    std::string titlex, std::string choose_textx,
    std::vector<text_menu *> optionsx, std::function<void(player &p)> actionx,
    bool is_choosex, std::function<void(player &p, int id)> choose_cbx,
    std::function<std::vector<std::string>(const player &p)> get_choose_setx,
    bool neb, std::string uidx)
    : title(titlex), choose_text(choose_textx), options(optionsx),
      is_choose(is_choosex), action(actionx), choose_cb(choose_cbx),
      get_choose_set(get_choose_setx), need_back(neb)
{
    father = nullptr;
    for (text_menu *px : optionsx) {
        px->father = this;
    }
    if (uid.length() != 0) {
        set_uid(uidx);
    }
}

bool text_menu::set_uid(std::string u)
{
    if (uid.length() != 0) {
        text_menu_mapper.erase(uid);
    }
    if (text_menu_mapper.find(u) != text_menu_mapper.end()) {
        fmt::print("Cannot set uid: {}, already beed used.", u);
        return false;
    }
    uid = u;
    if (uid.length() == 0)
        return true;
    text_menu_mapper[u] = this;
    return true;
}

std::string text_menu::get_uid() const { return uid; }

void text_menu::add_option(text_menu *p)
{
    p->father = this;
    this->options.push_back(p);
}

void text_menu::add_app_option(const std::string &typ, text_menu *p)
{
    p->father = this;
    this->app_options[typ].push_back(p);
}
void text_menu::add_app_options(const std::string &typ,
                                std::vector<text_menu *> p)
{
    for (auto it : p) {
        it->father = this;
        this->app_options[typ].push_back(it);
    }
}

std::string text_menu::to_string(const player &p) const
{
    std::string ret;
    if (title.length() != 0) {
        ret += title + '\n';
    }
    int cnt = 0;
    if (is_choose) {
        for (auto it : get_choose_set(p)) {
            ret += std::to_string(cnt) + ". " + it + '\n';
            cnt++;
        }
    }
    else {
        for (auto it : options) {
            ret += std::to_string(cnt) + ". " + it->choose_text + '\n';
            cnt++;
        }
        for (auto it : app_options) {
            if (p.is_type(it.first)) {
                for (auto it2 : it.second) {
                    ret += std::to_string(cnt) + ". " + it2->choose_text + '\n';
                    cnt++;
                }
            }
        }
    }
    if (need_back)
        ret += std::to_string(cnt) + ". 返回";
    return trim(ret);
}

const text_menu *text_menu::get_nxt_menu(player &p, int id) const
{
    if (id < 0) {
        p.output2user("ni gan ma~~haha aiyo");
        return this;
    }
    if (is_choose) {
        if (id < static_cast<int>(get_choose_set(p).size())) {
            if (choose_cb != nullptr) {
                choose_cb(p, id);
            }
            if (options.size() < 1 || options[0] == nullptr) {
                fmt::print("Error: choose menu dont have default option\n");
            }
            if (options[0]->action != nullptr) {
                options[0]->action(p);
            }
            return options[0];
        }
        else if (need_back &&
                 id == static_cast<int>(get_choose_set(p).size())) {
            if (father->action != nullptr) {
                father->action(p);
            }
            return father;
        }
        else {
            return this;
        }
    }
    if (id < static_cast<int>(options.size())) {
        if (options[id]->action != nullptr) {
            options[id]->action(p);
        }
        return options[id];
    }
    else {
        id -= options.size();
        for (auto it : app_options) {
            if (p.is_type(it.first)) {
                if (id < static_cast<int>(it.second.size())) {
                    if (it.second[id]->action != nullptr) {
                        it.second[id]->action(p);
                    }
                    return it.second[id];
                }
                else {
                    id -= it.second.size();
                }
            }
        }
        if (father != nullptr && id == 0 && need_back) {
            if (father->action != nullptr) {
                father->action(p);
            }
            return father;
        }
        else {
            return this;
        }
    }
}

text_menu *root_menu;

std::map<std::string, std::function<void(player &, int)>>
    choose_callback_mapper = {
        {"menu_choose_pokemon_chest_pkm",
         [](player &p, int id) { p.mt.menu_choose_pokemon = id + 6; }},
        {"set_choose_pkm",
         [](player &p, int id) { p.mt.menu_choose_pokemon = id; }},
        {"set_choose_id", [](player &p, int id) { p.mt.menu_choose_id = id; }},
        {"set_choose_item",
         [](player &p, int id) { p.mt.menu_choose_item = id; }},
        {"set_choose_position",
         [](player &p, int id) { p.mt.menu_choose_position = id; }},
        {"set_battle_change",
         [](player &p, int id) { p.mt.battle_change_pkm = id; }}};

std::map<std::string, std::function<void(player &)>> action_mapper = {
    {"output_pkm_state",
     [](player &p) {
         const pkm *u = p.get_choose_pkm_const();
         std::string s1 =
             fmt::format("{} 的性格\n {} 在 {} 遇到了它。",
                         nature2str[u->nature], u->get_time, u->get_place);
         std::string s2 = fmt::format(
             "{} Lv. {}, {}, 持有物：{}", u->get_name(), u->level,
             gender2string(u->gend), u->carried_item); // TOOD: item here
         std::string s3 = fmt::format(
             "No. {}, type: {}, 现有经验值: {}, 距下一级经验值: {}", u->id,
             eletype2string(u->typ), u->exp_curr,
             get_next_level_exp(u->exp_acc_speed, u->level) - u->exp_curr);
         p.output2user(fmt::format("{}\n\n{}\n\n{}", s1, s2, s3));
     }},
    {"output_place_point",
     [](player &p) {
         p.output2user(fmt::format("距去往下一地点还需：{} 点数",
                                   p.pls->meet_points - p.mt.move_point));
     }},
    {"output_place_desc",
     [](player &p) {
         p.output2user(fmt::format("{}\n{}", p.pls->name, p.pls->desc));
     }},
    {"reset_menu_choose_pokemon",
     [](player &p) { p.mt.menu_choose_pokemon = -1; }},
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
    {"set_default_choose_position",
     [](player &p) { p.mt.menu_choose_position = 0; }},
    {"battle_wild_pkm",
     [](player &p) {
         auto cmp = [](const pkm &a, const pkm &b) {
             return a.level < b.level;
         };
         pkm u = pkm::create_pkm(pkm_list[get_possi_random(p.pls->pkms)], "",
                                 get_random(7) - 5 +
                                     std::max_element(p.party_pkm.begin(), p.party_pkm.end(), cmp)->level,
                                 "", p.pls->name);
         player *wp = new player(u);
         int result = battle_start(&p, wp, 1);
         delete wp;
         p.sig_save = true;
         p.mt.move_point += 1;
         if (p.mt.move_point >= p.pls->meet_points)
             p.st.user_enables.insert("enough_place_point");
     }},
    {"move_to_next_place",
     [](player &p) {
         if (p.mt.move_point >= p.pls->meet_points) {
             p.mt.move_point = 0;
             if (p.mt.move_point >= p.pls->meet_points)
                 p.st.user_enables.insert("enough_place_point");
             else
                 p.st.user_enables.erase("enough_place_point");
             p.pls = p.pls->neighbors[p.mt.menu_choose_id];
             p.output2user(get_action_text("move_to_next_place", p));
         }
         else {
             // TODO: text here
         }
     }},
    {"set_is_goback_true", [](player &p) { p.mt.is_goback = true; }},
    {"battle_target_choose", [](player &p) {
         base_skill bs = skill_list[p.party_pkm[p.mt.menu_choose_pokemon]
                                        .skills[p.mt.menu_choose_id]];
         if (p.bm->battle_num == 1 || is_no_target_skill(bs.tgt)) {
             p.mt.menu_choose_position = 0;
         }
         else {
             run_text_menu(p, battle_target_choose_menu, nullptr, root_menu);
         }
     }}};

std::map<std::string, std::function<std::vector<std::string>(const player &)>>
    get_choose_set_mapper = {
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
                 ret.push_back(skill_list[it].name);
             }
             return ret;
         }},
        {"get_pkm_skill_target_list",
         [](const player &p) -> std::vector<std::string> {
             std::vector<std::string> ret;
             base_skill bs = skill_list[p.party_pkm[p.mt.menu_choose_pokemon]
                                            .skills[p.mt.menu_choose_id]];
             if (is_no_target_skill(bs.tgt)) {
                 ret.push_back("hi, bugs here XD");
             }
             else {
                 if (bs.tgt == move_target::ANY_ALLY) {
                     int side = (&p == p.bm->p[1] ? 1 : 0);
                     for (auto it : p.bm->pkms[side]) {
                         if (it != &p.party_pkm[p.mt.menu_choose_pokemon]) {
                             if (it != nullptr) {
                                 ret.push_back(it->get_name());
                             }
                             else {
                                 ret.push_back("empty");
                             }
                         }
                     }
                 }
                 else if (bs.tgt == move_target::ANY_OPPO) {
                     int side = (&p == p.bm->p[1] ? 1 : 0);
                     for (auto it : p.bm->pkms[side ^ 1]) {
                         if (it != nullptr) {
                             ret.push_back(it->get_name());
                         }
                         else {
                             ret.push_back("empty");
                         }
                     }
                 }
                 else if (bs.tgt == move_target::SELECTED_OPPO) {
                     int side = (&p == p.bm->p[1] ? 1 : 0);
                     int position;
                     for (position = 0;
                          position < p.bm->pkms[side ^ 1].size() &&
                          p.bm->pkms[side ^ 1][position] !=
                              &p.party_pkm[p.mt.menu_choose_pokemon];
                          ++position)
                         ;
                     for (int i = -1; i <= 1; i++) {
                         if (p.bm->check_valid_pkm(side ^ 1, i + position)) {
                             ret.push_back(p.bm->pkms[side ^ 1][i + position]
                                               ->get_name());
                         }
                         else {
                             ret.push_back("empty");
                         }
                     }
                 }
                 else {
                 }
             }
             return ret;
         }}};
////// Main menu(up), first pkm choose menu(down)

text_menu *first_pkm_choose_menu;

std::vector<std::string> get_init_pkm_list(const player &p)
{
    std::vector<std::string> ret;
    for (int i : first_pkm_list) {
        ret.push_back(pkm_list[i].base_name);
    }
    return ret;
}

text_menu *yes_confirm_menu(void (*act)(player &p))
{
    return new text_menu("", "是", {}, act);
}
text_menu *no_confirm_menu() { return new text_menu("", "否", {}); }
text_menu *choose_confirm_menu(text_menu *f, void (*act)(player &p))
{
    text_menu *p = new text_menu("确定？", "", {yes_confirm_menu(act)});
    p->options[0]->father = f;
    return p;
}

void choose_first_pkm_action(player &p)
{
    if (p.party_pkm.size() < 6) {
        p.party_pkm.push_back(
            pkm::create_pkm(pkm_list[first_pkm_list[p.mt.menu_choose_pokemon]],
                            name_pkm(p), 5, nowtime_string(), p.pls->name));
    }
}

void pkm_ch_init(const text_menu *f)
{
    first_pkm_choose_menu = new text_menu(
        "选择初始伙伴", "", {}, nullptr, true,
        map_finder((std::string) "set_choose_pkm", choose_callback_mapper),
        get_init_pkm_list, false);
    first_pkm_choose_menu->add_option(
        choose_confirm_menu(first_pkm_choose_menu, choose_first_pkm_action));
    first_pkm_choose_menu->father = f;
    first_pkm_choose_menu->options[0]->options[0]->father = f;
}

text_menu *player_name_init_menu;

void name_ch_action(player &p)
{
    p.output2user("请输入名字：");
    p.name = p.get_user_input();
}

text_menu *choose_name_menu(const text_menu *f)
{
    text_menu *p =
        new text_menu("名字是 $u，这样可以吗？", "Next",
                      {yes_confirm_menu(nullptr), no_confirm_menu()},
                      name_ch_action, false, nullptr, nullptr, false);
    p->options[0]->father = f;
    return p;
}

text_menu *battle_menu;
text_menu *battle_target_choose_menu;
text_menu *subsitute_menu;
void init_battle_menu(const text_menu *f)
{
    auto whatx = new text_menu();
    whatx->father = f;
    subsitute_menu = new text_menu(
        "请选择替换的宝可梦", "", {whatx}, nullptr, true,
        map_finder((std::string) "set_choose_pkm", choose_callback_mapper),
        map_finder((std::string) "get_player_available_pkm_name_list",
                   get_choose_set_mapper),
        false);
}

std::vector<std::pair<text_menu *, std::string>> menu_fathers;
std::vector<std::pair<text_menu *, std::string>> menu_options;
void init_main_menu(const Json::Value &J)
{
    auto *p = J2text_menu(J);
    if (p->get_uid() == "main_menu") {
        root_menu = p;
    }
    else if (p->get_uid() == "main_battle_menu") {
        battle_menu = p;
    }
    else if (p->get_uid() == "battle_target_choose") {
        battle_target_choose_menu = p;
    }
    else {
        if (!J.isMember("father") && p->father == nullptr) {
            fmt::print("Warning: No father for menu:{}", J.toStyledString());
        }
    }
}

void link_menu_options()
{
    for (auto it : menu_options) {
        auto it2 = text_menu_mapper.find(it.second);
        if (it2 == text_menu_mapper.end()) {
            fmt::print("WARNING: menu_id: {} cannot be found!\n", it.second);
        }
        else {
            it.first->add_option(it2->second);
        }
    }
}

void link_menu_father()
{
    for (auto it : menu_fathers) {
        auto it2 = text_menu_mapper.find(it.second);
        if (it2 == text_menu_mapper.end()) {
            fmt::print("WARNING: menu_id: {} cannot be found!\n", it.second);
        }
        else {
            it.first->father = it2->second;
        }
    }
}

void menu_init()
{
    Json::Value Ja = string_to_json(readfile("./data/pkm/menu.json"));
    for (Json::Value J : Ja) {
        init_main_menu(J);
    }
    pkm_ch_init(root_menu);
    init_battle_menu(root_menu);
    link_menu_options();
    link_menu_father();
    // manually or read from file both ok
}

void _menu_remove(text_menu *p)
{
    for (auto it : p->options) {
        _menu_remove(it);
    }
    for (auto it : p->app_options) {
        for (auto it2 : it.second) {
            _menu_remove(it2);
        }
    }
    delete p;
}

void menu_remove()
{
    _menu_remove(root_menu);
    _menu_remove(first_pkm_choose_menu);
    _menu_remove(player_name_init_menu);
    _menu_remove(battle_menu);
}