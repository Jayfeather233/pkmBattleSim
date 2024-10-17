#pragma once

#include "menu.hpp"
#include "utils.h"

#include <atomic>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <string>

class singleplayerthread {
public:
    userid_t user_id;
    player p;
    bot *botp;
    std::atomic<bool> sig_ipt;
    std::string ipt;
    msg_meta user_msg_conf;

    bool in_group_event; // only enable if player agree

    void add_input(const std::string &ipt, const uint64_t group_id = 0)
    {
        this->ipt = ipt;
        sig_ipt.store(true);
        // just store the last operation
    }
    singleplayerthread(uint64_t user_id, bot *ptr = nullptr)
        : user_id(user_id), p("./config/pkm/" + std::to_string(user_id) + ".json"), botp(ptr),
          user_msg_conf("private", user_id, 0, 0, botp)
    {
        this->p.get_user_input = [this]() { return this->get_user_input(true); };
        this->p.get_user_input_no_wait = [this]() { return this->get_user_input(false); };
        this->p.output2user = [this](std::string s) { return this->output2user(s); };
        this->p.is_op = [user_id, ptr]() { return is_op(ptr, user_id); };
        in_group_event = false;
        if (this->p.party_pkm.size() == 0) {
            init_player();
        }
        if (p.mt.move_point >= p.pls->meet_points)
            p.st.user_enables.insert("enough_place_point");
        else
            p.st.user_enables.erase("enough_place_point");
    }

    void init_player()
    {
        player_name_init();
        choose_init_pkm();
    }

    void player_name_init()
    {
        run_text_menu(p, player_name_init_menu, [this]() { this->save(); }, root_menu);
    }
    void choose_init_pkm()
    {
        auto it = user_specific_pkm_idmap.find(this->user_id);
        if (it != user_specific_pkm_idmap.end()) {
            run_text_menu(p, first_pkm_specific_menu, [this]() { this->save(); }, root_menu);
        }
        else {
            run_text_menu(p, first_pkm_choose_menu, [this]() { this->save(); }, root_menu);
        }
        save();
    }

    void run()
    {
        run_text_menu(p, root_menu, [this]() { this->save(); }, nullptr);
    }

    std::string get_user_input(bool is_wait = true)
    {
        if (botp == nullptr) {
            fmt::print("Input: ");
            std::string ret;
            std::getline(std::cin, ret);
            return ret;
        }
        else {
            if (is_wait) {
                while (!sig_ipt.load() || in_group_event) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                    if (sig_ipt.load()) {
                        output2user("Cannot interactive during group event.");
                        sig_ipt.store(false);
                    }
                }
                sig_ipt.store(false);
                return ipt;
            }
            else {
                if (sig_ipt.load()) {
                    sig_ipt.store(false);
                    return ipt;
                }
                else {
                    return "";
                }
            }
        }
    }

    void output2user(const std::string &s)
    {
        if (botp == nullptr) {
            if (s.length() > 0)
                std::cout << s << std::endl;
        }
        else {
            if (s.length() > 0)
                botp->cq_send(s, user_msg_conf);
        }
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
    void save() { this->p.save("./config/pkm/" + std::to_string(user_id) + ".json"); }
};

#ifdef QQBOT
class multiplayerthread {
public:
    userid_t player1, player2;
    player *p1, *p2;
    bot *p;
};
#endif