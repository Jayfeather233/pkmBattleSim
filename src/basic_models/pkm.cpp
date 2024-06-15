#include "pkm.hpp"

pkm_info *get_pkm_info(int id) { return &pkm_info_maper[id]; }

float get_nature_rate(int nature) { return 1.0; }