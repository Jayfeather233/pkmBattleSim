#pragma once

#include "processable.h"
#include "threads.hpp"

// Need to start thread for each user and group_battle

#include <map>
#include <thread>

std::map<uint64_t, singleplayerthread *> th_mapper;

class pkmbattle : public processable {
private:
public:
    void process(std::string message, const msg_meta &conf)
    {
        auto it = th_mapper.find(conf.user_id);
        if (it == th_mapper.end()) {
            th_mapper.insert(std::pair<const uint64_t, singleplayerthread *>(
                conf.user_id, new singleplayerthread(conf.user_id, conf.p)));
            th_mapper[conf.user_id]->add_input(
                message.substr(4),
                conf.message_type == "group" ? conf.group_id : 0);
        }
        else {
            th_mapper[conf.user_id]->add_input(
                message.substr(4),
                conf.message_type == "group" ? conf.group_id : 0);
        }
    }
    bool check(std::string message, const msg_meta &conf)
    {
        return message.find("pkm.") == 0;
    }
    std::string help() {}
    ~pkmbattle()
    {
        for (auto it : th_mapper) {
            delete it.second;
        }
    }
};

extern "C" processable *create()
{
    init_predefs();
    return new pkmbattle();
}
extern "C" void close() { remove_predefs(); }

//TODO: when group op changes its group type, update all online players setting.
//TODO: players setting update using group setting when player get online