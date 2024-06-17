#include "player.hpp"
#include "utils.hpp"

void player::save(const std::string &filepath){
    Json::Value J;
    J["badge"] = this->badge;
    J["party_pkm"] = pkm2Ja(this->party_pkm);
    J["chest_pkm"] = pkm2Ja(this->chest_pkm);
    J["pls"] = this->pls->id;
    J["money"] = this->money;
    writefile(filepath, J.toStyledString(), false);
}
player player::load(const std::string &filepath){
    Json::Value J = string_to_json(readfile(filepath, "{\"badge\":0,\"party_pkm\":[],\"chest_pkm\":[],\"pls\":0,\"money\":0}"));
    return (player){J["badge"].asInt(), Ja2pkm(J["party_pkm"]), Ja2pkm(J["chest_pkm"]), &placs[J["pls"].asInt()], J["money"].asInt(), nullptr, nullptr, nullptr, nullptr, -1};
}