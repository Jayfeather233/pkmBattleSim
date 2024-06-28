#include "player.hpp"
#include "utils.hpp"

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

player player::load(const std::string &filepath)
{
    Json::Value J = string_to_json(readfile(
        filepath, "{\"badge\":0,\"name\": \"\", "
                  "\"party_pkm\":[],\"chest_pkm\":[],\"pls\":0,\"money\":0, "
                  "\"settings\":{\"type\":[],\"enable\":[]}}"));
    return player(J["badge"].asInt(), J["name"].asString(),
                  Ja2pkm(J["party_pkm"]), Ja2pkm(J["chest_pkm"]),
                  &placs[J["pls"].asInt()], J["money"].asInt(), nullptr,
                  nullptr, nullptr,
                  settings(Ja2set(J["settings"]["type"]),
                           Ja2set(J["settings"]["enable"])));
}

bool settings::is_type(const std::string &tp) const
{
    return user_types.find(tp) != user_types.end() &&
           user_enables.find(tp) != user_enables.end();
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
        return chest_pkm.size() > mt.menu_choose_pokemon - 6
                   ? &chest_pkm[mt.menu_choose_pokemon - 6]
                   : &chest_pkm[0];
    }
    else {
        return party_pkm.size() > mt.menu_choose_pokemon
                   ? &party_pkm[mt.menu_choose_pokemon]
                   : &party_pkm[0];
    }
}
const pkm *player::get_choose_pkm_const() const
{
    if (mt.menu_choose_pokemon > 6) {
        return chest_pkm.size() > mt.menu_choose_pokemon - 6
                   ? &chest_pkm[mt.menu_choose_pokemon - 6]
                   : &chest_pkm[0];
    }
    else {
        return party_pkm.size() > mt.menu_choose_pokemon
                   ? &party_pkm[mt.menu_choose_pokemon]
                   : &party_pkm[0];
    }
}

player::player(int bad, std::string name, std::vector<pkm> pp,
               std::vector<pkm> cp, places *pl, int mon,
               std::function<std::string()> gui,
               std::function<void(const std::string &)> opu,
               std::function<bool()> iop, settings sts)
    : badge(bad), name(name), party_pkm(pp), chest_pkm(cp), pls(pl), money(mon),
      st(sts), get_user_input(gui), output2user(opu), is_op(iop),
      mt{-1}
{
    sig_save = false;
}