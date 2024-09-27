#pragma once

#include "bot.h"
#include "player.hpp"

#include <string>
#include <vector>

class text_menu {
private:
    std::string uid;
    std::string title;

public:
    std::string choose_text; // display at father manu's option list
    text_menu const *father;
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
    std::function<std::vector<std::string>(const player &p)>
        get_choose_set; // call to get item list
    bool need_back;     // the default back menu

    bool set_uid(std::string u);
    std::string get_uid() const;
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
extern std::map<userid_t, int> user_specific_pkm_idmap;
extern text_menu *first_pkm_specific_menu;
extern std::map<std::string, std::map<std::string, std::vector<text_menu *>>>
    app_menu_mapper; // uid->(type->menu*)
extern std::map<std::string, text_menu *>
    text_menu_mapper; // when add a menu, it will store uid->menu*
extern text_menu *player_name_init_menu;
extern text_menu *battle_menu;
extern text_menu *battle_target_choose_menu;
extern text_menu *subsitute_menu;
extern std::vector<std::pair<text_menu *, std::string>> menu_fathers;
extern std::vector<std::pair<text_menu *, std::string>> menu_options;

void menu_init();
void menu_remove();

void run_text_menu(player &p, const text_menu *px, std::function<void()> save,
                   const text_menu *stop = nullptr);

extern std::map<std::string, std::function<void(player &, int)>>
    choose_callback_mapper;

extern std::map<std::string, std::function<void(player &)>> action_mapper;

extern std::map<std::string,
                std::function<std::vector<std::string>(const player &)>>
    get_choose_set_mapper;