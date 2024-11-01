#pragma once

#include "processable.h"
#include "threads.hpp"

// Need to start thread for each user and group_battle

#include <map>
#include <thread>

class pkmbattle : public processable {
private:
    std::map<userid_t, singleplayerthread *> th_mapper;
    std::map<userid_t, bool> user_continues_input;
public:
    void process(std::string message, const msg_meta &conf);
    bool check(std::string message, const msg_meta &conf);
    std::string help();
    ~pkmbattle();
};

DECLARE_FACTORY_FUNCTIONS_HEADER

// TODO: when group op changes its group type, update all online players setting.
// TODO: players setting update using group setting when player get online