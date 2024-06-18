#pragma once

#include "player.hpp"

#include <string>
#include <vector>

class text_menu {
private:
    int uid;
public:
    std::string title;
    std::string choose_text; // display at father manu's option list
    text_menu *father;
    std::vector<text_menu *> options;
    std::map<std::string, std::vector<text_menu *>>
        app_options; // appended options for user_type, like Pokémon Refresh in
                     // Sun/Moon, this can allow users to choose what features
                     // to enable. Since it only involves simple status change,
                     // this can be read from file

    bool is_choose;            // is a choose menu
    std::function<void(player &p)> action; // the action take when getting to this menu
    void (*choose_cb)(player &p, int id);  // choose callback with id
    bool need_back; // the default back menu
    std::vector<std::string> (*get_choose_set)(
        player &p); // call to get item list

    bool set_uid(int u);
    text_menu();

    text_menu(std::string titlex, std::string choose_textx,
              std::vector<text_menu *> optionsx,
              void (*actionx)(player &p) = nullptr, bool is_choosex = false,
              void (*choose_cbx)(player &p, int id) = nullptr,
              std::vector<std::string> (*get_choose_setx)(player &p) = nullptr, int uidx=0);

    void add_option(text_menu *p);
    void add_app_option(const std::string &typ, text_menu *p);
    void add_app_options(const std::string &typ, std::vector<text_menu *> p);

    std::string to_string(player &p);

    text_menu *get_nxt_menu(player &p, int id);
};

extern text_menu *root_menu;
extern text_menu *first_pkm_choose_menu;
extern std::map<int, std::map<std::string, std::vector<text_menu*>>> app_menu_mapper;
extern std::map<int, text_menu*> text_menu_mapper;

void menu_init();