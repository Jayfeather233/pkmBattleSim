#include "menu.hpp"

#include "random.hpp"
#include <fmt/core.h>

std::map<std::string, std::map<std::string, std::vector<text_menu *>>> app_menu_mapper;

std::string name_pkm(player &p, bool gender)
{
    p.output2user(
        fmt::format("想给{:s}起名字吗？\n0. 是\n1. 否", gender ? "她" : "他"));
    if (p.get_user_input() == "0") {
        p.output2user(fmt::format("请输入名字："));
        return p.get_user_input();
    }
    else {
        return "";
    }
}

text_menu::text_menu() { father = nullptr; }

text_menu::text_menu(std::string titlex, std::string choose_textx,
                     std::vector<text_menu *> optionsx,
                     void (*actionx)(player &p), bool is_choosex,
                     void (*choose_cbx)(player &p, int id),
                     std::vector<std::string> (*get_choose_setx)(player &p),
                     std::string uidx)
    : title(titlex), choose_text(choose_textx), options(optionsx),
      is_choose(is_choosex), action(actionx), choose_cb(choose_cbx),
      get_choose_set(get_choose_setx)
{
    father = nullptr;
    for (text_menu *px : optionsx) {
        px->father = this;
    }
    need_back = true;
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
    if(uid.length() == 0) return true;
    text_menu_mapper[u] = this;
    return true;
}

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

std::string text_menu::to_string(player &p)
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
    return ret;
}

text_menu *text_menu::get_nxt_menu(player &p, int id)
{
    if (id < 0) {
        p.output2user("ni gan ma~~haha aiyo");
        return this;
    }
    if (is_choose) {
        if (id < get_choose_set(p).size()) {
            if (choose_cb != nullptr) {
                choose_cb(p, id);
            }
            if (options[0]->action != nullptr) {
                options[0]->action(p);
            }
            return options[0];
        }
        else {
            if (father->action != nullptr) {
                father->action(p);
            }
            return father;
        }
    }
    if (id < options.size()) {
        if (options[id]->action != nullptr) {
            options[id]->action(p);
        }
        return options[id];
    }
    else {
        id -= options.size();
        for (auto it : app_options) {
            if (p.is_type(it.first)) {
                if (id < it.second.size()) {
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
        if (father != nullptr) {
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

void set_choose_pkm(player &p, int id) { p.menu_choose_pokemon = id; }
void set_choose_ch_pkm(player &p, int id) { p.menu_choose_pokemon = id + 6; }

std::vector<std::string> get_player_party_pkm_name_list(player &p)
{
    std::vector<std::string> ret;
    for (auto pk : p.party_pkm) {
        if (pk.id != 0) {
            ret.push_back(pk.get_name());
        }
    }
    return ret;
}
std::vector<std::string> get_player_chest_pkm_name_list(player &p)
{
    std::vector<std::string> ret;
    for (auto it : p.chest_pkm) {
        if (it.id != 0) {
            ret.push_back(it.get_name());
        }
    }
    return ret;
}

text_menu *init_pkm_afterchoose_menu()
{
    text_menu *p = new text_menu("做什么呢？", "", {/* interactive options */});
    p->set_uid("1");
    return p;
}

text_menu *init_party_pkm_menu()
{
    return new text_menu("选择一个宝可梦", "选择同行宝可梦",
                         {init_pkm_afterchoose_menu()}, nullptr, true,
                         set_choose_pkm, get_player_party_pkm_name_list);
}
text_menu *init_chest_pkm_menu()
{
    return new text_menu("选择一个宝可梦", "选择箱子宝可梦",
                         {/* interactive options */}, nullptr, true,
                         set_choose_ch_pkm, get_player_chest_pkm_name_list);
}

text_menu *init_pkm_choose_menu()
{
    text_menu *p= new text_menu("选择一个宝可梦", "选择宝可梦",
                         {init_party_pkm_menu(), init_chest_pkm_menu()});
    p->set_uid("2");
    return p;
}

text_menu *init_item_menu()
{
    return new text_menu("选择一个物品", "选择物品",
                         {/* different items */});
}

// text_menu *init_pkm_menu()
// {
//     text_menu *p = new text_menu("pokemon menu", "pokemon menu", {}, false,
//                                  nullptr, nullptr, nullptr);
//     p->add_option(init_pkm_choose_menu());
//     p->add_option(init_item_menu());
//     return p;
// }
// text_menu *init_pkm_menu()
// {
//     text_menu *p = new text_menu("pokemon menu", "pokemon menu", {}, false,
//                                  nullptr, nullptr, nullptr);
//     p->add_option(init_pkm_choose_menu());
//     p->add_option(init_item_menu());
//     return p;
// }

text_menu *init_player_menu()
{
    return new text_menu("玩家菜单", "玩家菜单",
                         {init_pkm_choose_menu(), init_item_menu()});
}
text_menu *init_places_menu()
{
    return new text_menu(
        "地点菜单", "地点菜单",
        {/* move, meet wild pkm, meet wild trainer, meet gym*/});
}

////// Main menu(up), first pkm choose menu(down)

text_menu *first_pkm_choose_menu;

std::vector<std::string> get_init_pkm_list(player &p)
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
        bool gen = get_random(2);
        p.party_pkm.push_back(
            pkm::create_pkm(pkm_list[first_pkm_list[p.menu_choose_pokemon]],
                            name_pkm(p, gen), gen, 5));
    }
}

void pkm_ch_init(text_menu *f)
{
    first_pkm_choose_menu =
        new text_menu("选择初始伙伴", "", {}, nullptr, true,
                      set_choose_pkm, get_init_pkm_list);
    first_pkm_choose_menu->add_option(
        choose_confirm_menu(first_pkm_choose_menu, choose_first_pkm_action));
    first_pkm_choose_menu->father = f;
    first_pkm_choose_menu->options[0]->options[0]->father = f;
}

void menu_init()
{
    root_menu = new text_menu("", "", {init_player_menu(), init_places_menu()});
    pkm_ch_init(root_menu);
    // add manually because of the action pointer;
}