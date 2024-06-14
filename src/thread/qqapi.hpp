#pragma once

#ifdef QQBOT
#include "processable.h"
#include "threads.hpp"

// Need to start thread for each user and group_battle

#include <map>
#include <thread>

std::map<std::pair<uint64_t, uint64_t>, singleplayerthread> th_mapper;

class pkmbattle : public processable {
private:
public:
    void process(std::string message, const msg_meta &conf) {

        auto it = th_mapper.find(conf.group_id, conf.user_id);
        if (it == th_mapper.end()) {
            th_mapper.insert(
                std::make_pair(std::make_pair(conf.group_id, conf.user_id),
                               singleplayerthread(conf.user_id, conf.p)));
            th_mapper[std::make_pair(conf.group_id, conf.user_id)].add_input(
                message.substr(4));
        }
        else {
            th_mapper[std::make_pair(conf.group_id, conf.user_id)].add_input(
                message.substr(4));
        }
    }
    bool check(std::string message, const msg_meta &conf) {
        return message.find("pkm.") == 0;
    }
    std::string help() {}
};

#else
#endif
