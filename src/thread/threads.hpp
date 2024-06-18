#pragma once

#include "menu.hpp"

#include "menu.hpp"

#include <atomic>
#include <cstdint>
#include <iostream>
#include <string>

class singleplayerthread {
public:
    uint64_t user_id;
    text_menu *main_memu;
    player p;
#ifdef QQBOT
    msg_meta conf;
    std::atomic<bool> sig_ipt;
    std::string ipt;
    void add_input(const std::string &ipt)
    {
        this->ipt = ipt;
        sig_ipt.store(true);
        // just store the last operation
    }
#endif
    singleplayerthread(uint64_t user_id, void *ptr = nullptr)
        : user_id(user_id),
          p(player::load("./config/pkm/" + std::to_string(user_id) + ".json"))
    {
#ifdef QQBOT
        this->conf = (msg_meta){"private", user_id, 0, 0, ptr};
#endif
        this->main_memu = root_menu;
        this->p.get_user_input = [this]() { return this->get_user_input(); };
        this->p.output2user = [this](std::string s) {
            return this->output2user(s);
        };
        this->p.is_op = [this]() { return false; };
        if (this->p.party_pkm.size() == 0) {
            choose_init_pkm();
        }
    }

    void choose_init_pkm()
    {
        text_menu *px = first_pkm_choose_menu;
        while (px != root_menu) {
            output2user(px->to_string(p));
            px = px->get_nxt_menu(p, get_next_int());
        }
        save();
    }

    void run()
    {
        while (1) {
            output2user(main_memu->to_string(p));
            main_memu = main_memu->get_nxt_menu(p, get_next_int());
        }
    }

#ifdef QQBOT
    std::string get_user_input()
    {
        while (!sig_ipt.load()) {
            std::this_thread::sleep_for(500ms);
        }
        sig_ipt.store(false);
        return ipt;
    }

    void output2user(const std::string &s)
    {
        if (s.length() > 0)
            conf.p->cq_send(s, conf);
    }

    int get_next_int(std::string text = "")
    {
        if (text.length()) {
            text = "\n" + text;
        }
        while (1) {
            try {
                return std::stoi(get_user_input());
            }
            catch (...) {
                output2user("invalid input." + text);
            }
        }
    }
#else
    std::string get_user_input()
    {
        std::string ret;
        std::getline(std::cin, ret);
        return ret;
    }

    void output2user(const std::string &s)
    {
        if (s.length() > 0)
            std::cout << s << std::endl;
    }

    int get_next_int(std::string text = "")
    {
        if (text.length()) {
            text = "\n" + text;
        }
        while (1) {
            try {
                return std::stoi(get_user_input());
            }
            catch (...) {
                output2user("invalid input." + text);
            }
        }
    }
#endif
    void save()
    {
        this->p.save("./config/pkm/" + std::to_string(user_id) + ".json");
    }
};

#ifdef QQBOT
class multiplayerthread {};
#endif