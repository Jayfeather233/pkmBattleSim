#include "menu.hpp"
#include "places.hpp"
#include "skills.hpp"
#include "utils.hpp"

#include <jsoncpp/json/json.h>
#include <vector>

#include <vector>

std::vector<region> regis;
std::vector<places> placs;
std::vector<pkm_info> pkm_info_maper;
std::vector<pkm_base> pkm_list;
std::vector<int> first_pkm_list;

void init_pkm(std::string filepath)
{
    pkm_list.push_back((pkm_base){});
    pkm_info_maper.push_back((pkm_info){});
    Json::Value J = string_to_json(readfile(filepath, "[]"));
    for (Json::Value item : J) {
        pkm_list.push_back(J2pkm_base(item));

        pkm_info_maper.push_back((pkm_info){
            item["id"].asInt(),
            readImage(item["image"].asString()), Ja2VecP(item["ava_skills"])});
    }
}

void init_places(std::string filepath)
{
    Json::Value J = string_to_json(readfile(filepath, "{}"));
    for (Json::Value re : J["regions"]) {
        regis.push_back((region){re["name"].asString(), re["desc"].asString()});
        region *f = &regis[regis.size() - 1];
        size_t st_pos = placs.size();
        for (Json::Value plcs : re["places"]) {
            placs.push_back((places){plcs["name"].asString(),
                                     plcs["desc"].asString(),
                                     f,
                                     plcs["points"].asInt(),
                                     {},
                                     Ja2Vec2(plcs["pkms"]),
                                     placs.size()});
        }
        for (Json::Value conn : re["connect"]) {
            int from = conn["from"].asInt(), to = conn["to"].asInt();
            placs[st_pos + from].neighbors.push_back(&placs[st_pos + to]);
            placs[st_pos + to].neighbors.push_back(&placs[st_pos + from]);
        }
    }
}

void init_predefs()
{
    init_skills();
    init_pkm("./data/pkm/pkm.json");
    init_places("./data/pkm/place.json");

    menu_init();
}
