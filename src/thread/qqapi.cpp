#include "qqapi.hpp"

void pkmbattle::process(std::string message, const msg_meta &conf)
{
    auto it = th_mapper.find(conf.user_id);
    if (it == th_mapper.end()) {
        if(message != "pkm.start") return;
        th_mapper.insert(std::pair<const uint64_t, singleplayerthread *>(conf.user_id,
                                                                         new singleplayerthread(conf.user_id, conf.p)));
        // th_mapper[conf.user_id]->add_input(message.substr(4), conf.message_type == "group" ? conf.group_id : 0);
        th_mapper[conf.user_id]->run();
    }
    else {
        if(message == "pkm.resend"){
            th_mapper[conf.user_id]->resend();
            return;
        }
        th_mapper[conf.user_id]->add_input(message.substr(4), conf.message_type == "group" ? conf.group_id : 0);
    }
}
bool pkmbattle::check(std::string message, const msg_meta &conf) { return message.find("pkm.") == 0; }
std::string pkmbattle::help() {}
pkmbattle::~pkmbattle()
{
    for (auto it : th_mapper) {
        delete it.second;
    }
}

extern "C" processable *create_t()
{
    init_predefs();
    return new pkmbattle();
}
extern "C" void destroy_t(processable *p)
{
    remove_predefs();
    delete p;
}