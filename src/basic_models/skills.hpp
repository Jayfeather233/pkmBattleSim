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
    move_target tgt;
    int prior; // level of priority, like: 电光一闪 +1，镜面反射 -5
    virtual void affect(pkm &self, pkm &p, bool &is_critical) {}
    virtual void affect(field_status &fs) {}
    base_skill(size_t uid, const std::string &nm, const std::string &ds,
               char ar, char hr, u_char pp, move_target tgx, int pri)
        : uid(uid), name(nm), desc(ds), affect_range(ar), hitrate(hr), pp(pp),
          tgt(tgx), prior(pri)
    {
    }
    base_skill(const base_skill &bs)
        : uid(bs.uid), name(bs.name), desc(bs.desc),
          affect_range(bs.affect_range), hitrate(bs.hitrate), pp(bs.pp),
          tgt(bs.tgt), prior(bs.prior)
    {
    }
    base_skill(const base_skill &&bs)
        : uid(bs.uid), name(bs.name), desc(bs.desc),
          affect_range(bs.affect_range), hitrate(bs.hitrate), pp(bs.pp),
          tgt(bs.tgt), prior(bs.prior)
    {
    }
};

// like reduce the defence something
class aff_move : public base_skill {
public:
    base6 aff_points;
    char aff_hitrate;
    char aff_evasionrate;
    [[override]] void affect(pkm &self, pkm &p, bool &is_critical)
    {
        p.bstate6.affected += aff_points;
        p.hpreduced += p.bstate6.affected.hp;
        p.bstate6.affected.hp = 0;
        p.bstate6.affected_hitrate += aff_hitrate;
        p.bstate6.affected_evasionrate += aff_evasionrate;
    }
    aff_move(const base_skill &bs, const base6 &aff_pts, const char &aff_hr,
             const char &aff_eva)
        : base_skill(bs), aff_points(aff_pts), aff_hitrate(aff_hr),
          aff_evasionrate(aff_eva)
    {
    }
    aff_move() : base_skill(0, "", "", 0, 0, 0, static_cast<move_target>(0), 0)
    {
    }
};

class field_move : public base_skill {
public:
    char succ_rate;
    field_status aff_to;
    field_move(const base_skill &bs, const char &sr, const field_status &at)
        : base_skill(bs), succ_rate(sr), aff_to(at)
    {
    }
    [[override]] void affect(field_status &fs)
    {
        if ((fs == field_status::TOXIC_SPICKES ||
             fs == field_status::TOXIC_SPICKES2) &&
            aff_to == field_status::TOXIC_SPICKES) {
            fs = field_status::TOXIC_SPICKES2;
        }
        else if (fs == field_status::SPIKES && aff_to == field_status::SPIKES) {
            fs = field_status::SPIKES2;
        }
        else if (fs == field_status::SPIKES2 &&
                 aff_to == field_status::SPIKES) {
            fs = field_status::SPIKES3;
        }
        else if (fs == field_status::SPIKES3 &&
                 aff_to == field_status::SPIKES) {
            fs = field_status::SPIKES3;
        }
        else {
            fs = aff_to;
        }
    }
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
    [[override]] void affect(pkm &self, pkm &p, bool &is_critical)
    {
        float eft = eff_table[tp][p.typ[0]] * eff_table[tp][p.typ[1]];
        is_critical =
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

    atk_move(size_t uid, const std::string nm, const std::string ds, char ar,
             char hr, u_char pp, move_target tgx, int prio, element_types et,
             bool son, u_char atkx, bool easy_cri, side_effect w)
        : base_skill(uid, nm, ds, ar, hr, pp, tgx, prio), tp(et),
          sp_or_norm(son), atk(atkx), is_easy_critical(easy_cri), se(w)
    {
    }
};

// predefined skills
extern std::vector<base_skill> skill_list;
void init_skills();
