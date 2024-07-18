#pragma once

#include "defs.hpp"
#include "menu.hpp"
#include "pkm.hpp"

#include <jsoncpp/json/json.h>
#include <set>

#ifndef QQBOT
#include <filesystem>
#include <fstream>
#include <iostream>

std::fstream openfile(const std::string file_path,
                      const std::ios_base::openmode mode);
std::string readfile(const std::string &file_path,
                     const std::string &default_content);
void writefile(const std::string file_path, const std::string &content,
               bool is_append);
Json::Value string_to_json(const std::string &str);

#endif

image readImage(std::string filepath);

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