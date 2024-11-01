#include "qqapi.hpp"

static std::string prefix = "pkm.";
static std::string start_ = "start";
static std::string continues_input = "begin";
static std::string end_input = "end";
static std::string resend = "resend";
static std::string help_ = "help";

static std::string help_msg =
"Online Pokemon simulator.\n"
"  pkm.start to create your account\n"
"  pkm.begin to start input\n"
"  pkm.end   to end input\n"
"  pkm.resend to resend last message\n"
"Notice this is in early development.\n"
"See: https://github.com/Jayfeather233/pkmBattleSim/";

void pkmbattle::process(std::string message, const msg_meta &conf)
{
    if(message.find(prefix) == 0) message = message.substr(prefix.length());
    if(message == continues_input){
        user_continues_input[conf.user_id] = true;
        conf.p->cq_send("进入连续输入模式，pkm.end来结束", conf);
        return;
    }
    if(message == end_input){
        user_continues_input[conf.user_id] = false;
        conf.p->cq_send("结束连续输入模式", conf);
        return;
    }
    if(message == help_){
        conf.p->cq_send(help_msg, conf);
        return;
    }
    auto it = th_mapper.find(conf.user_id);
    if (it == th_mapper.end()) {
        if (message != start_)
            return;
        user_continues_input[conf.user_id] = true;
        conf.p->cq_send("进入连续输入模式，pkm.end来结束", conf);
        th_mapper.insert(
            std::pair<userid_t, singleplayerthread *>(conf.user_id, new singleplayerthread(conf.user_id, conf.p)));
        th_mapper[conf.user_id]->init_player();
        th_mapper[conf.user_id]->run();
    }
    else {
        if (message == resend) {
            th_mapper[conf.user_id]->resend();
            return;
        }
        th_mapper[conf.user_id]->add_input(message, conf.message_type == "group" ? conf.group_id : 0);
    }
}
bool pkmbattle::check(std::string message, const msg_meta &conf)
{
    return message.find(prefix) == 0 || user_continues_input[conf.user_id];
}
std::string pkmbattle::help() { return "Online pkm game. See pkm.help"; }
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
    delete p;
    remove_predefs();
}