#pragma once

#include <map>
#include <string>
#include <vector>

#include "pkm.hpp"
#include "random.hpp"

class base_skill {
public:
    size_t uid;
    std::string name;
    std::string desc;
    char affect_range;
    char hitrate;
    u_char pp;
    virtual void affect(pkm &self, pkm &p) {}
    base_skill(size_t uid, const std::string nm, const std::string ds, char ar,
               char hr, u_char pp)
        : uid(uid), name(nm), desc(ds), affect_range(ar), hitrate(hr), pp(pp)
    {
    }
    base_skill(const base_skill &bs)
        : uid(bs.uid), name(bs.name), desc(bs.desc),
          affect_range(bs.affect_range), hitrate(bs.hitrate), pp(bs.pp)
    {
    }
    base_skill(const base_skill &&bs)
        : uid(bs.uid), name(bs.name), desc(bs.desc),
          affect_range(bs.affect_range), hitrate(bs.hitrate), pp(bs.pp)
    {
    }
};

// the description after using a skill
// like: $1 use scratch, hit $2.
// Need to split this, into:
//     use skill: $1 use scratch
//     skill hit: hit $2
//     hit result: very useful
std::string get_hitdesc(const base_skill &mv, const pkm &self,
                        const pkm &other);

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
    aff_move(const base_skill &bs, base6 aff_pts, char aff_hr, char aff_eva)
        : base_skill(bs), aff_points(aff_pts), aff_hitrate(aff_hr),
          aff_evasionrate(aff_eva)
    {
    }
    aff_move() : base_skill(0, "", "", 0, 0, 0) {}
};

// just attack
class atk_move : public base_skill {
public:
    element_types tp;
    bool sp_or_norm; // false for physical attack
    u_char atk;
    bool is_easy_critical;

    struct side_effect {
        bool self_ot_other; // true for other
        float prob;
        aff_move afm;
    } se;
    void affect(pkm &self, pkm &p)
    {
        float eft = eff_table[tp][p.typ[0]] * eff_table[tp][p.typ[1]];
        bool is_critical =
            (is_easy_critical ? (get_random(8) == 0) : (get_random(16) == 0));
        float app =
            eft * (is_critical ? 1.5 : 1.0) * (get_random(16) + 85) / 100.0;
        if (eft <= 1e-6)
            return;
        if (sp_or_norm)
            p.hpreduced += std::max(
                1, int(((2.0 * self.level + 10) / 250.0 * self.stat.stk * 1.0 /
                            p.stat.sdf * this->atk +
                        2) *
                       ((self.typ[0] == tp || self.typ[1] == tp) ? 1.5 : 1.0) *
                       app));
        else
            p.hpreduced += std::max(
                1, int(((2.0 * self.level + 10) / 250.0 * self.stat.atk * 1.0 /
                            p.stat.df * this->atk +
                        2) *
                       ((self.typ[0] == tp || self.typ[1] == tp) ? 1.5 : 1.0) *
                       app));
    }

    atk_move(size_t uid, const std::string nm, const std::string ds, char ar, char hr, u_char pp, 
             element_types et, bool son, u_char atkx, bool easy_cri,
             side_effect w)
        : base_skill(uid, nm, ds, ar, hr, pp), tp(et), sp_or_norm(son), atk(atkx),
          is_easy_critical(easy_cri), se(w)
    {
    }
};

// predefined skills
extern std::vector<base_skill> skill_list;
void init_skills();
