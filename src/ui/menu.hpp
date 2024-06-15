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

    text_menu(){
        father = nullptr;
    }

    text_menu(std::string titlex, std::string choose_textx,
              std::vector<text_menu *> optionsx, bool is_choosex,
              void (*actionx)(player &p), void (*choose_cbx)(player &p, int id),
              std::vector<std::string> (*get_choose_setx)(player &p))
        : title(titlex), choose_text(choose_textx), options(optionsx),
          is_choose(is_choosex), action(actionx), choose_cb(choose_cbx),
          get_choose_set(get_choose_setx)
    {
        father = nullptr;
    }

    void add_option(text_menu *p)
    {
        p->father = this;
        this->options.push_back(p);
    }

    std::string to_string()
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

    text_menu *get_nxt_menu(player &p, int id)
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
};

extern text_menu root_menu;

void menu_init();