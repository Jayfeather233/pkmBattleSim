#include "skills.hpp"
#include "myutils.hpp"

// std::string get_hitdesc(const base_skill &mv, const pkm &self, const pkm &other)
// {
//     std::string dd = mv.hitdesc[get_random(mv.hitdesc.size())];
//     replaceAll(dd, "$1", self.get_name());
//     replaceAll(dd, "$2", other.get_name());
//     replaceAll(dd, "$$", "$");
//     return dd;
// }

std::vector<base_skill *> skill_list;

void init_skills()
{
    skill_list.push_back(new base_skill(0, "", "", 0b01110, 100, 35, move_target::ANY, 0));
    skill_list.push_back(new atk_move(1, "拍击", "使用长长的尾巴或手等拍打对手进行攻击。", 0b01110, 100, 35,
                                      move_target::ANY_OPPO, 0, element_types::PLAIN, false, 40, false,
                                      (atk_move::side_effect){false, 0, aff_move()}));
    // read from file then
}

void remove_skills()
{
    for (auto u : skill_list) {
        delete u;
    }
}