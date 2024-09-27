#include "menu.hpp"
#include "places.hpp"
#include "skills.hpp"
#include "utils.h"
#include "myutils.hpp"
#include "image_utils.hpp"

#include <iostream>
#include <filesystem>
#include <fmt/core.h>
#include <jsoncpp/json/json.h>
#include <vector>

std::vector<region> regis;
std::vector<places> placs;
std::vector<pkm_info> pkm_info_maper;
std::vector<pkm_base> pkm_list;
std::vector<int> first_pkm_list;
std::map<std::string, text_menu *> text_menu_mapper;
std::map<std::string, std::vector<std::string>> action_text;
std::map<userid_t, int> user_specific_pkm_idmap;

void init_user_specific_pkm(std::string filepath){
    Json::Value J = string_to_json(readfile(filepath, "{}"));
    for(std::string u : J.getMemberNames()){
        user_specific_pkm_idmap[my_string2uint64(u)] = J[u].asInt();
    }
}

void init_pkm(std::string filepath)
{
    pkm_list.push_back((pkm_base){});
    pkm_info_maper.push_back((pkm_info){});
    Json::Value J = string_to_json(readfile(filepath, "[]"));
    for (Json::Value item : J) {
        if (static_cast<int>(pkm_list.size()) != item["id"].asInt()) {
            std::cout << "Warning: pkm order in json file is not the id order."
                      << std::endl;
        }
        pkm_list.push_back(J2pkm_base(item));

        pkm_info_maper.push_back((pkm_info){item["id"].asInt(),
                                            readImage(item["image"].asString()),
                                            Ja2VecP(item["ava_skills"]),Ja2Vec(item["ava_skills_machine"])});
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
                                     static_cast<int>(placs.size())});
        }
        for (Json::Value conn : re["connect"]) {
            int from = conn["from"].asInt(), to = conn["to"].asInt();
            placs[st_pos + from].neighbors.push_back(&placs[st_pos + to]);
            placs[st_pos + to].neighbors.push_back(&placs[st_pos + from]);
        }
    }
}

/*
J:{
"type1":{ // append type
    "id1":[
        { // the append option goes here
            "title":
            ...
        }
    ]
}
}
*/
static std::vector<std::pair<text_menu *, std::string>> father_setter;
void app_menu_init(const std::string &rt_dir)
{
    for (const auto &entry : std::filesystem::directory_iterator(rt_dir)) {
        if (entry.is_regular_file()) {
            Json::Value J = string_to_json(readfile(entry.path(), "{}"));
            for (auto typ : J.getMemberNames()) {
                for (auto uidx : J[typ].getMemberNames()) {
                    for (auto it : J[typ][uidx]) {
                        text_menu *p = J2text_menu(it);
                        app_menu_mapper[uidx][typ].push_back(p);
                        if (it.isMember("father"))
                            father_setter.push_back(
                                std::make_pair(p, it["father"].asString()));
                    }
                }
            }
        }
    }
}
void init_texts(const std::string &rt_dir)
{
    for (const auto &entry : std::filesystem::directory_iterator(rt_dir)) {
        if (entry.is_regular_file()) {
            Json::Value J = string_to_json(readfile(entry.path(), "{}"));
            for (auto typ : J.getMemberNames()) {
                for (auto it : J[typ]) {
                    action_text[typ].push_back(it.asString());
                }
            }
        }
    }
}

void insert_app_menu()
{
    for (auto it : app_menu_mapper) {
        auto it2 = text_menu_mapper.find(it.first);
        if (it2 == text_menu_mapper.end()) {
            fmt::print("WARNING: menu_id: {} cannot be found!\n", it.first);
            for (auto it3 : it.second) {
                for(auto u : it3.second)
                    delete u;
            }
        }
        else {
            for (auto it3 : it.second) {
                it2->second->add_app_options(it3.first, it3.second);
            }
        }
    }
    for(auto it:father_setter){
        auto it2 = text_menu_mapper.find(it.second);
        if (it2 == text_menu_mapper.end()) {
            fmt::print("WARNING: menu_id: {} cannot be found!\n", it.second);
        }
        else {
            it.first->father = it2->second;
        }
    }
}

void init_predefs()
{
    app_menu_init("./data/pkm/app_menus");
    init_skills();
    init_pkm("./data/pkm/pkm.json");
    init_places("./data/pkm/place.json");

    menu_init();
    insert_app_menu();
    init_texts("./data/pkm/texts");

    init_user_specific_pkm("./config/pkm/user_specific.json");
    first_pkm_list.push_back(1);
}

void remove_predefs()
{
    menu_remove();
    remove_skills();
}
