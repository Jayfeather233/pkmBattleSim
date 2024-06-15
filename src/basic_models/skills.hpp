#pragma once

#include <string>
#include <vector>

#include "pkm.hpp"
#include "random.hpp"

class base_skill {
public:
    std::string name;
    std::string desc;
    std::vector<std::string> hitdesc; // need to split more precisely
    char affect_range;
    char hitrate;
    virtual void affect(pkm &self, pkm &p) = 0;
};

void replaceAll(std::string &str, const std::string &from,
                const std::string &to);

// the description after using a skill
// like: $1 use scratch, hit $2.
// Need to split this, into:
//     use skill: $1 use scratch
//     skill hit: hit $2
//     hit result: very useful
std::string get_hitdesc(const base_skill &mv, const pkm &self, const pkm &other);

bool can_affected(const base_skill &mv, battle_position bp);

// like reduce the defence something
class aff_move : public base_skill {
public:
    base6 aff_points;
    char aff_hitrate;
    char aff_evasionrate;
    void affect(pkm &self, pkm &p)
    {
        p.bstate.affected += aff_points;
        p.hpreduced += p.bstate.affected.hp;
        p.bstate.affected.hp = 0;
        p.bstate.affected_hitrate += aff_hitrate;
        p.bstate.affected_evasionrate += aff_evasionrate;
    }
};

// just attack
class atk_move : public base_skill {
public:
    element_types tp;
    bool sp_or_norm;
    u_char atk;

    struct side_effect {
        float prob;
        aff_move afm;
    };
    void affect(pkm &self, pkm &p) {}
};

// predefined skills
extern std::vector<base_skill> skill_list;
void init_skills();