#pragma once

#include "defs.hpp"
#include "pkm.hpp"

#ifndef QQBOT
#include <filesystem>
#include <fstream>
#include <iostream>
#include <jsoncpp/json/json.h>

std::fstream openfile(const std::string file_path,
                      const std::ios_base::openmode mode);
std::string readfile(const std::string &file_path,
                     const std::string &default_content);
void writefile(const std::string file_path, const std::string &content,
               bool is_append);
Json::Value string_to_json(const std::string &str);

#endif

image readImage(std::string filepath);

std::vector<int> Ja2Vec(const Json::Value &Ja);
std::vector<std::pair<int, float>> Ja2Vec2(const Json::Value &Ja);
base6 J2base6(const Json::Value &J);
pkm_base J2pkm_base(const Json::Value &J);
pkm J2pkm(const Json::Value &J);
std::vector<pkm> Ja2pkm(const Json::Value &Ja);

Json::Value base62J(const base6 &b);
Json::Value pkm_base2J(const pkm_base &p);
Json::Value pkm2J(const pkm &p);
Json::Value pkm2Ja(const std::vector<pkm> &vp);