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
    replaceAll(dd, "$1", get_pkm_info(self.id)->name);
    replaceAll(dd, "$2", get_pkm_info(other.id)->name);
    replaceAll(dd, "$$", "$");
    return dd;
}

bool can_affected(const base_skill &mv, battle_position bp)
{
    return (mv.affect_range & (1 << bp));
}