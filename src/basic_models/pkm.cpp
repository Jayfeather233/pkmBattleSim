#include "pkm.hpp"

#include "random.hpp"
// Default constructor
pkm_base::pkm_base() : height(0.0f), weight(0.0f), catch_rate(0), gender_ratio(-1), exp_acc_speed(0), base_exp(0), id(0)
{
    typ[0] = element_types::EMPTY;
    typ[1] = element_types::EMPTY;
}

// Parameterized constructor 1
pkm_base::pkm_base(const std::string &nm, base6 sp, std::array<element_types, 2> et, int idx, const std::string &cat,
                   const std::vector<std::pair<int, float>> &abilities, float h, float w, u_char cr, int gr, base6 abp,
                   u_char eas, int bexp, std::vector<std::pair<int, float>> poss_itemx)
    : base_name(nm), species_points(sp), typ(et), category(cat), poss_abilities(abilities), poss_items(poss_itemx),
      height(h), weight(w), catch_rate(cr), gender_ratio(gr), aquire_base_point(abp), exp_acc_speed(eas),
      base_exp(bexp), id(idx)
{
}

gender pkm_base::get_gender() const
{
    if (gender_ratio == -1) {
        return gender::NONE;
    }
    else {
        return get_random(8) < gender_ratio ? gender::MALE : gender::FEMALE;
    }
}

pkm_info *get_pkm_info(int id) { return &pkm_info_maper[id]; }

base6 get_nature_rate(int nature) { return nature_table6[nature]; }

int get_next_level_exp(u_char eas, u_char level)
{
    int level2 = level * level;
    int level3 = level * level2;
    int level4 = level2 * level2;
    switch (eas) {
    case 0:
        if (level <= 50)
            return -0.02 * level4 + 2 * level3;
        else if (level <= 68)
            return -0.01 * level4 + 1.5 * level3;
        else if (level <= 98)
            return 0.002 * level3 * int((1911 - 10 * level) / 3);
        else
            return -0.01 * level4 + 1.6 * level3;
    case 1:
        return 0.8 * level3;
    case 2:
        return level3;
    case 3:
        return 1.2 * level3 - 15 * level2 + 100 * level - 140;
    case 4:
        return 1.25 * level3;
    case 5:
        if (level <= 15)
            return 0.02 * level3 * int((level + 73) / 3);
        else if (level <= 36)
            return 0.02 * level4 + 0.28 * level3;
        else
            return 0.02 * level3 * int((level + 64) / 2);
    default:
        throw "No such exp gain speed";
    }
}
std::vector<size_t> get_pkm_init_skills(const pkm_base &p, int level)
{
    pkm_info *pi = get_pkm_info(p.id);
    std::vector<size_t> ret;
    for (int i = pi->skills_can_learn.size() - 1; i >= 0; i--) {
        if (pi->skills_can_learn[i].first <= level) {
            ret.push_back(pi->skills_can_learn[i].second);
            if (ret.size() == 4) {
                return ret;
            }
        }
    }
    return ret;
}

pkm::pkm(const pkm_base &pb, const std::string &namex, const std::string &get_t, const std::string &get_p, gender gen,
         u_char levelx, size_t exp_currx, base6 IVx, base6 bp, u_char fship, std::array<size_t, 4> skillx,
         std::array<int, 4> used_ppx, int naturex, bool is_sh, int caritem, int abi, int hpred, battle_status bs)
    : pkm_base(pb), name(namex), get_time(get_t), get_place(get_p), gend(gen), level(levelx), exp_curr(exp_currx),
      IV(IVx), base_points(bp), friendship(fship), skills(skillx), used_pp(used_ppx), nature(naturex), is_shiny(is_sh),
      carried_item(caritem), ability(abi), hpreduced(hpred), bstatus(bs)
{
    refresh_stat();
    refresh_bstate6();
    exp_total = 0;
    for (int i = 1; i < levelx; i++) {
        exp_total += get_next_level_exp(pb.exp_acc_speed, i);
    }
    exp_total += exp_curr;
}

pkm::pkm(const pkm_base &pb, const pkm &pp)
    : pkm_base(pb), name(pp.name), get_time(pp.get_time), get_place(pp.get_place), gend(pp.gend), level(pp.level),
      exp_curr(pp.exp_curr), exp_total(pp.exp_total), IV(pp.IV), base_points(pp.base_points), friendship(pp.friendship),
      skills(pp.skills), used_pp(pp.used_pp), nature(pp.nature), is_shiny(pp.is_shiny), carried_item(pp.carried_item),
      ability(pp.ability), hpreduced(pp.hpreduced), bstatus(pp.bstatus)
{
    refresh_stat();
    refresh_bstate6();
}

void pkm::refresh_stat()
{
    stat.hp = (int)((species_points.hp * 2 + IV.hp + (float)base_points.hp / 4) * level / 100.0 + 10 + level);
    base6 nature_fix = get_nature_rate(nature);
#define gst(a)                                                                                                         \
    (int)(((species_points.a * 2 + IV.a + (float)base_points.a / 4.0) * level / 100.0 + 5) * nature_fix.a / 10.0)
    stat.atk = gst(atk);
    stat.stk = gst(stk);
    stat.df = gst(df);
    stat.sdf = gst(sdf);
    stat.spd = gst(spd);
#undef gst
}
void pkm::refresh_bstate6()
{
    this->bstate6.affected = (base6){0, 0, 0, 0, 0, 0};
    this->bstate6.affected_hitrate = 0;
    this->bstate6.affected_evasionrate = 0;
}

pkm pkm::create_pkm(const pkm_base &p, const std::string &nm, u_char level, const std::string &get_time,
                    const std::string &place_name)
{
    auto skills = get_pkm_init_skills(p, level);
    while (skills.size() < 4)
        skills.push_back(0);
    return pkm(p, nm, get_time, place_name, p.get_gender(), level, 0, random_base6(32), (base6){0, 0, 0, 0, 0, 0}, 70,
               {skills[0], skills[1], skills[2], skills[3]}, {0, 0, 0, 0}, get_random(25), get_random(4096) == 0,
               get_possi_random(p.poss_items), get_possi_random_(p.poss_abilities), 0, battle_status::NORMAL);
}