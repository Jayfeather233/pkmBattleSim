#include "skills.hpp"
#include "utils.hpp"

// std::string get_hitdesc(const base_skill &mv, const pkm &self, const pkm &other)
// {
//     std::string dd = mv.hitdesc[get_random(mv.hitdesc.size())];
//     replaceAll(dd, "$1", self.get_name());
//     replaceAll(dd, "$2", other.get_name());
//     replaceAll(dd, "$$", "$");
//     return dd;
// }

bool can_affected(const base_skill &mv, battle_position bp)
{
    return (mv.affect_range & (1 << bp));
}

std::vector<base_skill> skill_list;

void init_skills()
{
    skill_list.push_back(base_skill(0, "", "",
                         0b01110, 100, 35));
    skill_list.push_back(atk_move(1, "拍击", "使用长长的尾巴或手等拍打对手进行攻击。",
                         0b01110, 100, 35, element_types::PLAIN, false, 40,
                         false, (atk_move::side_effect){false, 0, aff_move()}));
    // read from file then
}