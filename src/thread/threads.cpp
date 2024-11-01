#include "threads.hpp"

#include "menu.hpp"
#include "myutils.hpp"

#include <atomic>
#include <chrono>
#include <cstdint>
#include <iostream>

void singleplayerthread::add_input(const std::string &ipt, const uint64_t group_id)
{
    this->ipt = ipt;
    sig_ipt.store(true);
    // just store the last operation
}
singleplayerthread::singleplayerthread(userid_t user_id, bot *ptr)
    : user_id(user_id), p("./config/pkm/" + std::to_string(user_id) + ".json"), botp(ptr),
      user_msg_conf("private", user_id, 0, 0, ptr)
{
    this->p.get_user_input = [this]() { return this->get_user_input(true); };
    this->p.get_user_input_no_wait = [this]() { return this->get_user_input(false); };
    this->p.output2user = [this](std::string s) { return this->output2user(s); };
    this->p.is_op = [user_id, ptr]() { return is_op(ptr, user_id); };
    this->sig_ipt.store(false);
    this->last_sent = "";
    in_group_event = false;
}

void singleplayerthread::init_player()
{
    if (this->p.party_pkm.size() == 0) {
        fmt::print("new player: {}, run init\n", user_id);
        map_finder((std::string) "reset_all_menu_tmp", action_mapper)(p);
        player_init();
        map_finder((std::string) "reset_all_menu_tmp", action_mapper)(p);
        choose_init_pkm();
    }
    if (p.mt.move_point >= p.pls->meet_points)
        p.st.user_enables.insert("enough_place_point");
    else
        p.st.user_enables.erase("enough_place_point");
}

void singleplayerthread::player_init()
{
    run_text_menu(p, player_init_menu, [this]() { this->save(); }, root_menu);
}
void singleplayerthread::choose_init_pkm()
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

void singleplayerthread::run()
{
    run_text_menu(p, root_menu, [this]() { this->save(); }, nullptr);
}

std::string singleplayerthread::get_user_input(bool is_wait)
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
            if (in_group_event) {
                sig_ipt.store(false);
                return "";
            }
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

void singleplayerthread::output2user(const std::string &s) 
{
    if (s.length() == 0)
        return;
    last_sent = s;
    if (botp == nullptr) {
        std::cout << s << std::endl;
    }
    else {
        botp->cq_send(s, user_msg_conf);
    }
}
void singleplayerthread::resend() const {
    if (last_sent.length() == 0)
        return;
    if (botp == nullptr) {
        std::cout << last_sent << std::endl;
    }
    else {
        botp->cq_send(last_sent, user_msg_conf);
    }
}

void singleplayerthread::save() { this->p.save("./config/pkm/" + std::to_string(user_id) + ".json"); }