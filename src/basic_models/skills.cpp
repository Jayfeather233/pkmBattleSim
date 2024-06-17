#include "skills.hpp"

void replaceAll(std::string &str, const std::string &from,
                const std::string &to)
{
    size_t startPos = 0;
    while ((startPos = str.find(from, startPos)) != std::string::npos) {
        str.replace(startPos, from.length(), to);
        startPos += to.length(); // In case 'to' contains 'from', like replacing
                                 // 'x' with 'yx'
    }
}

std::string get_hitdesc(const base_skill &mv, const pkm &self, const pkm &other)
{
    std::string dd = mv.hitdesc[get_random(mv.hitdesc.size())];
    replaceAll(dd, "$1", self.name);
    replaceAll(dd, "$2", other.name);
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
        element_types::PLAIN, false, 50, (atk_move::side_effect){0, aff_move()}
    ));
}