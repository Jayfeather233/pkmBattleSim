#include "evolve.hpp"
#include "myutils.hpp"
#include "utils.h"

#include <jsoncpp/json/json.h>

// map: pkm_id -> [evolve methods]
// evolve method = [trigger, trigger arg, to pkm_id]
std::map<int, std::vector<std::tuple<evolve_trigger, int, int>>> evolve_chain;

std::pair<bool, pkm> check_evolve(const pkm &p, evolve_trigger et, int arg)
{
    auto it1 = evolve_chain.find(p.id);
    if (it1 == evolve_chain.end())
        goto no_evolve;
    for (auto &it2 : it1->second) {
        if (std::get<0>(it2) == et) {
            switch (et) {
            case level_up:
                if (arg >= std::get<1>(it2)) {
                    return std::make_pair(true, create_evolve(p, std::get<2>(it2)));
                }
                else {
                    goto no_evolve;
                }
                break;

            case user_item:
                if (arg == std::get<1>(it2)) {
                    return std::make_pair(true, create_evolve(p, std::get<2>(it2)));
                }
                else {
                    goto no_evolve;
                }
                break;

            case trade:
                return std::make_pair(true, create_evolve(p, std::get<2>(it2)));
                break;

            default:
                throw "No such evolve trigger";
                break;
            }
        }
    }
no_evolve:
    return std::make_pair(false, pkm());
}

pkm create_evolve(const pkm &p, int evolve_id) { return pkm(pkm_list[evolve_id], p); }

void init_evolve_chains(const std::filesystem::path &fp)
{
    // [
    //  {from_id: 123, to_id: 456, trigger: 0, arg: 0},
    // ]
    Json::Value J = string_to_json(readfile(fp, "[]"));
    for (auto it : J) {
        evolve_chain[it["from_id"].asInt()].emplace_back(static_cast<evolve_trigger>(it["trigger"].asInt()),
                                                         it["arg"].asInt(), it["to_id"].asInt());
    }
}