#include "player.hpp"
#include "evolve.hpp"
#include "myutils.hpp"
#include "texts.hpp"
#include "utils.h"

#include <thread>

void player::save(const std::string &filepath)
{
    Json::Value J;
    J["badge"] = this->badge;
    J["name"] = this->name;
    J["party_pkm"] = pkm2Ja(this->party_pkm);
    J["chest_pkm"] = pkm2Ja(this->chest_pkm);
    J["pls"] = this->pls->id;
    J["money"] = this->money;
    Json::Value J2;
    J2["type"] = set2Ja(st.user_types);
    J2["enable"] = set2Ja(st.user_enables);
    J["settings"] = J2;
    writefile(filepath, J.toStyledString(), false);
}

player::player(const std::string &filepath)
    : player(0, "", {}, {}, nullptr, 0, nullptr, nullptr, nullptr,
             settings({}, {}))
{
    Json::Value J = string_to_json(readfile(
        filepath, "{\"badge\":0,\"name\": \"\", "
                  "\"party_pkm\":[],\"chest_pkm\":[],\"pls\":0,\"money\":0, "
                  "\"settings\":{\"type\":[],\"enable\":[]}}"));

    badge = J["badge"].asInt();
    name = J["name"].asString();
    party_pkm = Ja2pkm(J["party_pkm"]);
    chest_pkm = Ja2pkm(J["chest_pkm"]);
    pls = &placs[J["pls"].asInt()];
    money = J["money"].asInt();
    st = settings(Ja2set(J["settings"]["type"]),
                  Ja2set(J["settings"]["enable"]));
    sig_save = false;
}

player::player(const pkm &p)
    : badge(-1), name(p.get_name()), party_pkm({p}), chest_pkm({}),
      pls(nullptr), money(0), st(settings()), get_user_input(nullptr),
      output2user(nullptr), is_op(nullptr), mt(menu_temp()), sig_save(false)
{
}
player::player(const std::string &name, const std::vector<pkm> &p)
    : badge(-2), name(name), party_pkm(p), chest_pkm({}), pls(nullptr),
      money(0), st(settings()), get_user_input(nullptr), output2user(nullptr),
      is_op(nullptr), mt(menu_temp()), sig_save(false)
{
}

bool settings::is_type(const std::string &tp) const
{
    return user_enables.find(tp) != user_enables.end();
}

settings::settings() {}
settings::settings(const std::set<std::string> &ut,
                   const std::set<std::string> &ue)
    : user_types(ut), user_enables(ue)
{
}

bool player::is_type(const std::string &tp) const { return st.is_type(tp); }

pkm *player::get_choose_pkm()
{
    if (mt.menu_choose_pokemon > 6) {
        return static_cast<int>(chest_pkm.size()) > mt.menu_choose_pokemon - 6
                   ? &chest_pkm[mt.menu_choose_pokemon - 6]
                   : &chest_pkm[0];
    }
    else {
        return static_cast<int>(party_pkm.size()) > mt.menu_choose_pokemon
                   ? &party_pkm[mt.menu_choose_pokemon]
                   : &party_pkm[0];
    }
}
const pkm *player::get_choose_pkm_const() const
{
    if (mt.menu_choose_pokemon > 6) {
        return static_cast<int>(chest_pkm.size()) > mt.menu_choose_pokemon - 6
                   ? &chest_pkm[mt.menu_choose_pokemon - 6]
                   : &chest_pkm[0];
    }
    else {
        return static_cast<int>(party_pkm.size()) > mt.menu_choose_pokemon
                   ? &party_pkm[mt.menu_choose_pokemon]
                   : &party_pkm[0];
    }
}

player::player(int bad, std::string name, std::vector<pkm> pp,
               std::vector<pkm> cp, places *pl, int mon,
               std::function<std::string(bool)> gui,
               std::function<void(const std::string &)> opu,
               std::function<bool()> iop, settings sts)
    : badge(bad), name(name), party_pkm(pp), chest_pkm(cp), pls(pl), money(mon),
      st(sts), get_user_input([gui]() { return gui(true); }),
      get_user_input_no_wait([gui]() { return gui(false); }), output2user(opu),
      is_op(iop), mt{-1}
{
    sig_save = false;
}

void player::gain_exp(size_t position, size_t exp)
{
    pkm &p = party_pkm[position];

    p.refresh_stat();
    base6 pst = p.stat;

    p.exp_curr += exp;
    p.exp_total += exp;
    int oo = p.level;
    int u;
    while (p.exp_curr >= (u = get_next_level_exp(p.exp_acc_speed, p.level))) {
        p.exp_curr -= u;
        p.level += 1;
    }
    if (p.level != oo) {
        p.refresh_stat();

        this->output2user(fmt::format(
            "{}\n"
            "{}: {} -> {}\n"
            "{}: {} -> {}\n"
            "{}: {} -> {}\n"
            "{}: {} -> {}\n"
            "{}: {} -> {}\n"
            "{}: {} -> {}",
            get_levelup_text(p), base6str[0], pst.hp, p.stat.hp, base6str[1],
            pst.atk, p.stat.atk, base6str[2], pst.df, p.stat.df, base6str[3],
            pst.stk, p.stat.stk, base6str[4], pst.sdf, p.stat.sdf, base6str[5],
            pst.spd, p.stat.spd));
        // TODO: evolve
        if (check_evolve(p, evolve_trigger::level_up, p.level)) {
            this->output2user(get_evolving_text(p));
            std::this_thread::sleep_for(std::chrono::seconds(5));
            if (this->get_user_input_no_wait() != "") {
                std::string ori_name = p.get_name();
                party_pkm[position] =
                    create_evolve(p, evolve_trigger::level_up, p.level);
                this->output2user(get_evolved_text(p, ori_name));
            }
            else {
                this->output2user(get_evolve_abort_text(p));
            }
        }
    }
}

int player::get_subsitute_pkm(const std::vector<pkm *> u)
{
    auto ret = get_available_pkm();
    if (ret.size() == 0)
        return -1;
    else {
        run_text_menu(*this, subsitute_menu, nullptr, root_menu);
        return this->mt.menu_choose_pokemon;
    }
}

std::vector<pkm *> player::get_available_pkm() const
{
    std::vector<pkm *> ret;
    for (auto u : this->party_pkm) {
        if (!IS_FAINT(&u)) {
            ret.push_back(&u);
        }
    }
    return ret;
}