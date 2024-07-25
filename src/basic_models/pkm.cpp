#include "pkm.hpp"

#include "random.hpp"
// Default constructor
pkm_base::pkm_base()
    : height(0.0f), weight(0.0f), catch_rate(0), gender_ratio(-1),
      exp_acc_speed(0), base_exp(0), id(0)
{
    typ[0] = element_types::EMPTY;
    typ[1] = element_types::EMPTY;
}

// Parameterized constructor 1
pkm_base::pkm_base(const std::string &nm, base6 sp,
                   std::array<element_types, 2> et, int idx,
                   const std::string &cat,
                   const std::vector<std::pair<int, float>> &abilities, float h,
                   float w, u_char cr, int gr, base6 abp, u_char eas, int bexp,
                   std::vector<std::pair<int, float>> poss_itemx)
    : base_name(nm), species_points(sp), typ(et), category(cat),
      poss_abilities(abilities), poss_items(poss_itemx), height(h), weight(w),
      catch_rate(cr), gender_ratio(gr), aquire_base_point(abp),
      exp_acc_speed(eas), base_exp(bexp), id(idx)
{
}

// Copy constructor
pkm_base::pkm_base(const pkm_base &u)
    : base_name(u.base_name), species_points(u.species_points),
      typ{u.typ[0], u.typ[1]}, category(u.category),
      poss_abilities(u.poss_abilities), height(u.height), weight(u.weight),
      catch_rate(u.catch_rate), gender_ratio(u.gender_ratio),
      aquire_base_point(u.aquire_base_point), exp_acc_speed(u.exp_acc_speed),
      base_exp(u.base_exp), id(u.id)
{
}

// Move constructor
pkm_base::pkm_base(pkm_base &&u)
    : base_name(std::move(u.base_name)),
      species_points(std::move(u.species_points)), typ{u.typ[0], u.typ[1]},
      category(std::move(u.category)),
      poss_abilities(std::move(u.poss_abilities)), height(u.height),
      weight(u.weight), catch_rate(u.catch_rate), gender_ratio(u.gender_ratio),
      aquire_base_point(std::move(u.aquire_base_point)),
      exp_acc_speed(u.exp_acc_speed), base_exp(u.base_exp), id(u.id)
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

base6 get_nature_rate(int nature)
{
    return base6(); // TODO: nature table here
}

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

pkm::pkm(const pkm_base &pb, const std::string &namex, gender gen,
         u_char levelx, int exp_needx, int exp_currx, base6 IVx, base6 bp,
         u_char fship, battle_status bs, std::array<size_t, 4> skillx,
         std::array<int, 4> used_ppx, int naturex, int hpred, bool is_sh,
         int caritem, int abi)
    : pkm_base(pb), name(namex), gend(gen), level(levelx), exp_need(exp_needx),
      exp_curr(exp_currx), IV(IVx), base_points(bp), friendship(fship), skills(skillx), used_pp(used_ppx),
      nature(naturex), is_shiny(is_sh), carried_item(caritem), ability(abi),
      hpreduced(hpred), bstatus(bs)
{
    refresh_stat();
}

void pkm::refresh_stat()
{
    stat.hp =
        (int)((species_points.hp * 2 + IV.hp + (float)base_points.hp / 4) *
                  level / 100.0 +
              10 + level);
    base6 nature_fix = get_nature_rate(nature);
#define gst(a)                                                                 \
    (int)(((species_points.a * 2 + IV.a + (float)base_points.a / 4.0) *        \
               level / 100.0 +                                                 \
           5) *                                                                \
          nature_fix.a / 10.0)
    stat.atk = gst(atk);
    stat.stk = gst(stk);
    stat.df = gst(df);
    stat.sdf = gst(sdf);
    stat.spd = gst(spd);
#undef gst
}
void pkm::refresh_bstate()
{
    this->bstate.affected = (base6){0, 0, 0, 0, 0, 0};
    this->bstate.affected_hitrate = 0;
    this->bstate.affected_evasionrate = 0;
}

pkm pkm::create_pkm(const pkm_base &p, const std::string &nm, u_char level)
{
    auto skills = get_pkm_init_skills(p, level);
    while (skills.size() < 4)
        skills.push_back(0);
    return pkm(p, nm, p.get_gender(), level,
               get_next_level_exp(p.exp_acc_speed, level), 0, random_base6(32),
               (base6){0, 0, 0, 0, 0, 0}, 70, battle_status::NORMAL,
               {skills[0], skills[1], skills[2], skills[3]}, {0, 0, 0, 0},
               get_random(25), 0, get_random(4096) == 0,
               get_possi_random(p.poss_items),
               get_possi_random_(p.poss_abilities));
}

pkm::pkm(const pkm &other)
    : pkm_base(other), name(other.name), gend(other.gend), level(other.level),
      exp_need(other.exp_need), exp_curr(other.exp_curr), IV(other.IV),
      base_points(other.base_points), friendship(other.friendship),
      nature(other.nature), is_shiny(other.is_shiny),
      carried_item(other.carried_item), ability(other.ability),
      stat(other.stat), hpreduced(other.hpreduced), bstatus(other.bstatus),
      bstate(other.bstate)
{
    // Copy arrays
    std::copy(std::begin(other.skills), std::end(other.skills),
              std::begin(skills));
    std::copy(std::begin(other.used_pp), std::end(other.used_pp),
              std::begin(used_pp));
}
pkm::pkm(const pkm &&other)
    : pkm_base(other), name(other.name), gend(other.gend), level(other.level),
      exp_need(other.exp_need), exp_curr(other.exp_curr), IV(other.IV),
      base_points(other.base_points), friendship(other.friendship),
      nature(other.nature), is_shiny(other.is_shiny),
      carried_item(other.carried_item), ability(other.ability),
      stat(other.stat), hpreduced(other.hpreduced), bstatus(other.bstatus),
      bstate(other.bstate)
{
    // Copy arrays
    std::copy(std::begin(other.skills), std::end(other.skills),
              std::begin(skills));
    std::copy(std::begin(other.used_pp), std::end(other.used_pp),
              std::begin(used_pp));
}