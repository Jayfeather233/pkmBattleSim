#include "menu.hpp"
#include "battle.hpp"
#include "myutils.hpp"
#include "random.hpp"
#include "texts.hpp"
#include "utils.h"
#include <fmt/core.h>

void run_text_menu(player &p, const text_menu *px, std::function<void()> save, const text_menu *stop)
{
    while (px != stop && px != nullptr) {
        while (px != stop && px != nullptr && px->no_choice(p)) {
            text_menu *pp = px->get_only_choice(p);
            if (pp != nullptr && pp->action != nullptr) {
                pp->action(p);
            }
            p.output2user(px->to_string(p));
            px = pp;
        }
        if (px == stop || px == nullptr) {
            break;
        }
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

std::map<std::string, std::map<std::string, std::vector<text_menu *>>> app_menu_mapper;

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
bool text_menu::no_choice(const player &p) const
{
    if (is_choose) {
        return false;
    }
    else {
        size_t u = options.size();
        for (auto it : app_options) {
            if (p.is_type(it.first)) {
                u += it.second.size();
            }
        }
        if (need_back) {
            return false;
        }
        return u == 1;
    }
}
text_menu* text_menu::get_only_choice(const player &p) const{
    
    if(options.size()){
        return options[0];
    }
    for (auto it : app_options) {
        if (p.is_type(it.first)) {
            if(it.second.size()){
                return it.second[0];
            }
        }
    }
    return nullptr;
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

text_menu::text_menu(std::string titlex, std::string choose_textx, std::vector<text_menu *> optionsx,
                     std::function<void(player &p)> actionx, bool is_choosex,
                     std::function<void(player &p, int id)> choose_cbx,
                     std::function<std::vector<std::string>(const player &p)> get_choose_setx, bool neb,
                     std::string uidx)
    : title(titlex), choose_text(choose_textx), options(optionsx), is_choose(is_choosex), action(actionx),
      choose_cb(choose_cbx), get_choose_set(get_choose_setx), need_back(neb)
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
        fmt::print("Cannot set uid: {}, already beed used.\n", u);
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
void text_menu::add_app_options(const std::string &typ, std::vector<text_menu *> p)
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
    else if(!no_choice(p)){
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
    ret = trim(ret);
    my_string_format(ret, p);
    return ret;
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
        else if (need_back && id == static_cast<int>(get_choose_set(p).size())) {
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
////// Main menu(up), first pkm choose menu(down)

text_menu *first_pkm_choose_menu;
text_menu *first_pkm_specific_menu; // TODO

std::vector<std::string> get_init_pkm_list(const player &p)
{
    std::vector<std::string> ret;
    for (int i : first_pkm_list) {
        ret.push_back(pkm_list[i].base_name);
    }
    return ret;
}

text_menu *yes_confirm_menu(void (*act)(player &p)) { return new text_menu("", "是", {}, act); }
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
        p.party_pkm.push_back(pkm::create_pkm(pkm_list[first_pkm_list[p.mt.menu_choose_pokemon]], name_pkm(p), 5,
                                              nowtime_string(), p.pls->name));
    }
}

void pkm_ch_init(const text_menu *f)
{
    first_pkm_choose_menu =
        new text_menu("选择初始伙伴", "", {}, nullptr, true,
                      map_finder((std::string) "set_choose_pkm", choose_callback_mapper), get_init_pkm_list, false);
    first_pkm_choose_menu->add_option(choose_confirm_menu(first_pkm_choose_menu, choose_first_pkm_action));
    first_pkm_choose_menu->father = f;
    first_pkm_choose_menu->options[0]->options[0]->father = f;
}

text_menu *player_init_menu;

void name_ch_action(player &p) { p.name = p.get_user_input(); }
void birthday_ch_action(player &p)
{
    // TODO
}

void init_player_init_menu(const text_menu *f)
{
    player_init_menu =
        new text_menu("", "",
                      {new text_menu("可以告诉我你的名字吗？", "",
                                     {new text_menu("你的生日是：", "", {new text_menu("这样就行了", "", {})},
                                                    birthday_ch_action, false, nullptr, nullptr, false)},
                                     name_ch_action, false, nullptr, nullptr, false)},
                      nullptr, false, nullptr, nullptr, false);
    player_init_menu->options[0]->options[0]->options[0]->father = f;
}

text_menu *battle_menu;
text_menu *battle_target_choose_menu;
text_menu *subsitute_menu;
void init_battle_menu(const text_menu *f)
{
    auto whatx = new text_menu();
    whatx->father = f;
    subsitute_menu =
        new text_menu("请选择替换的宝可梦", "", {whatx}, nullptr, true,
                      map_finder((std::string) "set_choose_pkm", choose_callback_mapper),
                      map_finder((std::string) "get_player_available_pkm_name_list", get_choose_set_mapper), false);
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
            fmt::print("Warning: No father for menu:{}, it maybe a NPC menu", J.toStyledString());
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

void init_menus(const std::string &rt_dir)
{
    for (const auto &entry : std::filesystem::directory_iterator(rt_dir)) {
        if (entry.is_regular_file()) {
            Json::Value Ja = string_to_json(readfile(entry.path(), "[]"));
            for (Json::Value J : Ja) {
                init_main_menu(J);
            }
        }
    }
}

void menu_init()
{
    // Json::Value Ja = string_to_json(readfile("./data/pkm/menu.json"));
    // for (Json::Value J : Ja) {
    //     init_main_menu(J);
    // }
    init_menus("./data/pkm/menu/");
    init_player_init_menu(root_menu);
    pkm_ch_init(root_menu);
    // TODO: first_pkm_specific_menu
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
    _menu_remove(first_pkm_specific_menu);
    _menu_remove(player_init_menu);
    _menu_remove(battle_menu);
}