#include "menu.hpp"

text_menu::text_menu() { father = nullptr; }

text_menu::text_menu(
    std::string titlex, std::string choose_textx,
    std::vector<text_menu *> optionsx, bool is_choosex,
    void (*actionx)(player &p),
    void (*choose_cbx)(player &p, int id),
    std::vector<std::string> (*get_choose_setx)(player &p))
    : title(titlex), choose_text(choose_textx), options(optionsx),
      is_choose(is_choosex), action(actionx), choose_cb(choose_cbx),
      get_choose_set(get_choose_setx)
{
    father = nullptr;
    for (text_menu *px : optionsx) {
        px->father = this;
    }
}

void text_menu::add_option(text_menu *p)
{
    p->father = this;
    this->options.push_back(p);
}

std::string text_menu::to_string()
{
    std::string ret;
    if (title.length() != 0) {
        ret += title + '\n';
    }
    int cnt = 0;
    for (auto it : options) {
        ret += std::to_string(cnt) + ". " + it->choose_text + '\n';
        cnt++;
    }
    ret += std::to_string(cnt) + ". back";
    return ret;
}

text_menu *text_menu::get_nxt_menu(player &p, int id)
{
    if (0 <= id && id < options.size()) {
        if (options[id]->action != nullptr) {
            options[id]->action(p);
        }
        return options[id];
    }
    else {
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

text_menu root_menu;

void set_choose_pkm(player &p, int id) { p.menu_choose_pokemon = id; }
std::vector<std::string> get_player_party_pkm_name_list(player &p)
{
    std::vector<std::string> ret;
    for (int i = 0; i < 6; i++) {
        if (p.party_pkm[i].id != 0) {
            ret.push_back(p.party_pkm[i].name);
        }
    }
    return ret;
}
std::vector<std::string> get_player_chest_pkm_name_list(player &p)
{
    std::vector<std::string> ret;
    for (auto it : p.chest_pkm) {
        if (it.id != 0) {
            ret.push_back(it.name);
        }
    }
    return ret;
}

text_menu *init_party_pkm_menu()
{
    return new text_menu("choose a pokemon", "choose a party pokemon",
                         {/* interactive options */}, true, nullptr,
                         set_choose_pkm, get_player_party_pkm_name_list);
}
text_menu *init_chest_pkm_menu()
{
    return new text_menu("choose a pokemon", "choose a party pokemon",
                         {/* interactive options */}, true, nullptr,
                         set_choose_pkm, get_player_chest_pkm_name_list);
}

text_menu *init_pkm_choose_menu()
{
    return new text_menu("choose a pokemon", "choose pokemon",
                         {init_party_pkm_menu(), init_chest_pkm_menu()}, false,
                         nullptr, nullptr, nullptr);
}

text_menu *init_item_menu()
{
    return new text_menu("choose an item", "choose item",
                         {/* different items */}, false, nullptr, nullptr,
                         nullptr);
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
    return new text_menu("player menu", "player menu",
                         {init_pkm_choose_menu(), init_item_menu()}, false,
                         nullptr, nullptr, nullptr);
}
text_menu *init_places_menu()
{
    return new text_menu(
        "place menu", "place menu",
        {/* move, meet wild pkm, meet wild trainer, meet gym*/}, false, nullptr,
        nullptr, nullptr);
}

void menu_init()
{
    root_menu = *new text_menu("", "", {init_player_menu(), init_places_menu()},
                               false, nullptr, nullptr, nullptr);
    // add manually because of the action pointer;
}