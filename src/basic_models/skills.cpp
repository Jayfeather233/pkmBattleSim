#include "skills.hpp"
#include "utils.hpp"

std::string get_hitdesc(const base_skill &mv, const pkm &self, const pkm &other)
{
    std::string dd = mv.hitdesc[get_random(mv.hitdesc.size())];
    replaceAll(dd, "$1", self.get_name());
    replaceAll(dd, "$2", other.get_name());
    replaceAll(dd, "$$", "$");
    return dd;
}

bool can_affected(const base_skill &mv, battle_position bp)
{
    return (mv.affect_range & (1 << bp));
}

std::vector<base_skill> skill_list;

void init_skills(){
    skill_list.push_back(aff_move());
    skill_list.push_back(atk_move(
        "撞击", "用身体用力地击打对手", {"$1向$2撞了过去！"}, 0b01110, 100,
        element_types::PLAIN, false, 50, false, (atk_move::side_effect){0, aff_move()}
    ));
}