#include "places.hpp"
#include "menu.hpp"
#include "skills.hpp"

#include <vector>

std::vector<region> regis;
std::vector<places> plcs;
std::vector<pkm_info> pkm_info_maper;
std::vector<pkm_base> pkm_list;
std::vector<base_skill> skill_list;

void init_predefs()
{
    // these should be loaded from file
    regis.push_back((region){"alolan", "a place called alolan"});

    plcs.push_back((places){"1st road", "the 1st road", &regis[0], 5, std::vector<places *>(), std::vector<std::pair<pkm_base, float>>()});

    pkm_info_maper.push_back((pkm_info){"empty", 0, nullptr});
    pkm_info_maper.push_back((pkm_info){
        "Bulbasaur", 1, nullptr});
    pkm_list.push_back((pkm_base){(base6){0, 0, 0, 0, 0, 0}, {EMPTY, EMPTY}, 0});
    pkm_list.push_back((pkm_base){
        (base6){45, 49, 49, 65, 65, 45},
        {GRASS, EMPTY},
        1});
    // skill_list.push_back()
    // these should be loaded from file

    menu_init();
}
