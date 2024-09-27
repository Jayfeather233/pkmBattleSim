#pragma once

#include "pkm.hpp"
#include <filesystem>
#include <map>
#include <vector>

enum evolve_trigger { level_up, user_item, trade };

// map: pkm_id -> [evolve methods]
// evolve method = [trigger, trigger arg, to pkm_id]
extern std::map<int, std::vector<std::tuple<evolve_trigger, int, int>>> evolve_chain;

std::pair<bool, pkm> check_evolve(const pkm &p, evolve_trigger et, int arg);

pkm create_evolve(const pkm &p, int evolve_id);

void init_evolve_chains(const std::filesystem::path &fp);