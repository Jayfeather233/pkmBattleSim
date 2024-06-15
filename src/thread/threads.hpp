#pragma once

#include "menu.hpp"

#include <atomic>
#include <cstdint>
#include <iostream>
#include <string>

class singleplayerthread {
public:
    uint64_t user_id;
    text_menu *now_memu;
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
    {
        this->user_id = user_id;
#ifdef QQBOT
        this->conf = (msg_meta){"private", user_id, 0, 0, ptr};
#endif
    }

    void run() {
        now_memu = &root_menu;
        // output2user(now_memu->to_string());
        
        while(1){
            output2user(now_memu->to_string());
            now_memu = now_memu->get_nxt_menu(p, get_next_int());
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

    void output2user(const std::string &s) { conf.p->cq_send(s, conf); }

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

    void output2user(const std::string &s) { std::cout << s << std::endl; }

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
};

#ifdef QQBOT
class multiplayerthread {};
#endif