#include "menu.hpp"

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
    text_menu *p =
        new text_menu("choose a pokemon", "choose a party pokemon",
                      {/* interactive options */}, true, nullptr,
                      set_choose_pkm, get_player_party_pkm_name_list);
    return p;
}
text_menu *init_chest_pkm_menu()
{
    text_menu *p =
        new text_menu("choose a pokemon", "choose a party pokemon",
                      {/* interactive options */}, true, nullptr,
                      set_choose_pkm, get_player_chest_pkm_name_list);
    return p;
}

text_menu *init_pkm_choose_menu()
{
    text_menu *p = new text_menu("choose a pokemon", "choose pokemon", {},
                                 false, nullptr, nullptr, nullptr);
    p->add_option(init_party_pkm_menu());
    p->add_option(init_chest_pkm_menu());
    return p;
}

text_menu *init_item_menu()
{
    text_menu *p =
        new text_menu("choose an item", "choose item", {/* different items */},
                      false, nullptr, nullptr, nullptr);
    return p;
}

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
    text_menu *p = new text_menu("player menu", "player menu", {}, false,
                                 nullptr, nullptr, nullptr);
    p->add_option(init_pkm_choose_menu());
    p->add_option(init_item_menu());
    return p;
}
text_menu *init_places_menu()
{
    text_menu *p =
        new text_menu("place menu", "place menu",
                      {/* move, meet wild pkm, meet wild trainer, meet gym*/},
                      false, nullptr, nullptr, nullptr);
    return p;
}

void menu_init()
{
    root_menu = text_menu("", "", {init_player_menu(), init_places_menu()},
                          false, nullptr, nullptr, nullptr);
    // add manually because of the action pointer;
}