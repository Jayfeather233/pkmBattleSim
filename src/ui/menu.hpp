#pragma once

#include "player.hpp"

#include <string>
#include <vector>

class text_menu {
public:
    std::string title;
    std::string choose_text; // display at father manu's option list
    text_menu *father;
    std::vector<text_menu *> options;
    bool is_choose;            // is a choose menu
    void (*action)(player &p); // the action take when getting to this menu
    void (*choose_cb)(player &p, int id); // choose callback with id
    std::vector<std::string> (*get_choose_set)(
        player &p); // call to get item list

    text_menu();

    text_menu(std::string titlex, std::string choose_textx,
              std::vector<text_menu *> optionsx,
              void (*actionx)(player &p) = nullptr,
              bool is_choosex = false,
              void (*choose_cbx)(player &p, int id) = nullptr,
              std::vector<std::string> (*get_choose_setx)(player &p) = nullptr);

    void add_option(text_menu *p);

    std::string to_string(player &p);

    text_menu *get_nxt_menu(player &p, int id);
};

extern text_menu *root_menu;
extern text_menu *first_pkm_choose_menu;

void menu_init();