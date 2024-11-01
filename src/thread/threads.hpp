#pragma once

#include "player.hpp"
#include "utils.h"

#include <string>

class singleplayerthread {
public:
    userid_t user_id;
    player p;
    bot *botp;
    std::atomic<bool> sig_ipt;
    std::string ipt;
    msg_meta user_msg_conf;
    std::string last_sent;

    bool in_group_event; // only enable if player agree

    void add_input(const std::string &ipt, const uint64_t group_id = 0);

    // please run init_player after each initialization
    singleplayerthread(userid_t user_id, bot *ptr = nullptr);

    void init_player();
    void player_init();
    void choose_init_pkm();

    void run();

    std::string get_user_input(bool is_wait = true);
    void output2user(const std::string &s);
    void resend() const;
    void save();
};

#ifdef QQBOT
class multiplayerthread {
public:
    userid_t player1, player2;
    player *p1, *p2;
    bot *p;
};
#endif