#pragma once

#include "player.hpp"

#include <string>
#include <vector>

class text_menu {
private:
    std::string uid;
    std::string title;

public:
    std::string choose_text; // display at father manu's option list
    text_menu *father;
    std::vector<text_menu *> options;
    std::map<std::string, std::vector<text_menu *>>
        app_options; // appended options for user_type, like Pokémon Refresh in
                     // Sun/Moon, this can allow users to choose what features
                     // to enable. Since it only involves simple status change,
                     // this can be read from file

    bool is_choose; // is a choose menu
    std::function<void(player &p)>
        action; // the action take when getting to this menu
    std::function<void(player &p, int id)> choose_cb; // choose callback with id
    bool need_back;                                   // the default back menu
    std::function<std::vector<std::string>(const player &p)>
        get_choose_set; // call to get item list

    bool set_uid(std::string u);
    std::string get_title(const player &p) const;
    text_menu();

    text_menu(std::string titlex, std::string choose_textx,
              std::vector<text_menu *> optionsx,
              std::function<void(player &p)> actionx = nullptr,
              bool is_choosex = false,
              std::function<void(player &p, int id)> choose_cbx = nullptr,
              std::function<std::vector<std::string>(const player &p)>
                  get_choose_setx = nullptr,
              bool need_back = true, std::string uidx = "");

    void add_option(text_menu *p);
    void add_app_option(const std::string &typ, text_menu *p);
    void add_app_options(const std::string &typ, std::vector<text_menu *> p);

    std::string to_string(const player &p) const;

    const text_menu *get_nxt_menu(player &p, int id) const;
    bool no_next_menu(const player &p) const;
};

extern text_menu *root_menu;
extern text_menu *first_pkm_choose_menu;
extern std::map<std::string, std::map<std::string, std::vector<text_menu *>>>
    app_menu_mapper;
extern std::map<std::string, text_menu *> text_menu_mapper;
extern text_menu *player_name_init_menu;
extern text_menu *battle_menu;

void menu_init();

void run_text_menu(player &p, const text_menu *px,std::function<void()>save,
                   const text_menu *stop = nullptr);