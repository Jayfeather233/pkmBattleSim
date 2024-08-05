#pragma once

#include "myutils.hpp"
#include "random.hpp"

// battle texts: start battle -> use skill -> switch pkm -> fainting -> end
// interactive texts: pkm interactive, item interactive, place interactivate

#include <map>
#include <string>
#include <vector>

extern std::map<std::string, std::vector<std::string>> action_text;

std::string get_action_text(const std::string &typ, const player &p1); // action in text_menu

std::string get_skill_text(const pkm &p1, const pkm &p2, int skill_id);

std::string get_entry_text(const pkm &p, int type); // entry type

std::string get_battle_start_text(const pkm &p); // TODO

std::string get_pkm_faint_text(const pkm &p);

std::string get_battle_end_text(const pkm &p);

std::string get_levelup_text(const pkm &p);

std::string get_evolving_text(const pkm &p);

std::string get_evolved_text(const pkm &p, const std::string &ori_name);

std::string get_evolve_abort_text(const pkm &p);