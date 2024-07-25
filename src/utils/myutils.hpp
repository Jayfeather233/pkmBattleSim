#pragma once

#include "menu.hpp"
#include "pkm.hpp"

#include <jsoncpp/json/json.h>
#include <set>
#include <fmt/core.h>

void replaceAll(std::string &str, const std::string &from,
                const std::string &to);

Json::Value set2Ja(const std::set<std::string> &s);
std::set<std::string> Ja2set(const Json::Value &Ja);
std::vector<int> Ja2Vec(const Json::Value &Ja);
std::vector<std::pair<int, int>> Ja2VecP(const Json::Value &Ja);
std::vector<std::string> Ja2VecS(const Json::Value &Ja);
std::vector<std::pair<int, float>> Ja2Vec2(const Json::Value &Ja);
base6 J2base6(const Json::Value &J);
pkm_base J2pkm_base(const Json::Value &J); // only used in init pkm_base, other
                                           // times using: pkm_list[id]
pkm J2pkm(const Json::Value &J);
std::vector<pkm> Ja2pkm(const Json::Value &Ja);
text_menu *J2text_menu(const Json::Value &J);

Json::Value base62J(const base6 &b);
Json::Value pkm2J(const pkm &p);
Json::Value pkm2Ja(const std::vector<pkm> &vp);

int get_next_int(player &p, std::string text = "");

template<typename K, typename T>
T map_finder(const K& name, std::map<K, T> &maper) {
    auto it = maper.find(name);
    if (it == maper.end()) {
        throw std::runtime_error(fmt::format("Cannot find key '{}' in the map<{} -> {}>",
                                             name, typeid(K).name(), typeid(T).name()));
    } else {
        return it->second;
    }
}

std::string gender2string(const gender &g);
std::string eletype2string(const element_types &a);
std::string eletype2string(const std::array<element_types, 2> a);
void my_string_format(std::string &s, const player &pl, const pkm* pk = nullptr);