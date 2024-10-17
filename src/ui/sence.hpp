#pragma once

#include "defs.hpp"
#include "pkm.hpp"

image get_battle_image(const pkm &self, const pkm &others) { ; }

extern std::vector<image> battle_bg_imgaes;
extern std::vector<image> interact_bg_imgaes;

void init_backgrounds() {}