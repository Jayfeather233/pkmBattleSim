#pragma once

#include "utils.hpp"
#include "random.hpp"

// battle texts: start battle -> use skill -> switch pkm -> fainting -> end
// interactive texts: pkm interactive, item interactive, place interactivate

#include <map>
#include <string>
#include <vector>

extern std::map<std::string, std::vector<std::string>> action_text;

std::string get_action_text(const std::string &typ, const pkm &p);