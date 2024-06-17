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
                   const std::string &cat, const std::vector<std::pair<int, float>> &abilities,
                   float h, float w, u_char cr, int gr, base6 abp, u_char eas,
                   int bexp)
    : base_name(nm), species_points(sp), typ{et[0], et[1]}, category(cat),
      poss_abilities(abilities), height(h), weight(w), catch_rate(cr),
      gender_ratio(gr), aquire_base_point(abp), exp_acc_speed(eas),
      base_exp(bexp), id(idx)
{
}

// Parameterized constructor 2
pkm_base::pkm_base(const std::string &nm, base6 sp, element_types et[2],
                   int idx, const std::string &cat,
                   const std::vector<std::pair<int, float>> &abilities, float h, float w,
                   u_char cr, int gr, base6 abp, u_char eas, int bexp)
    : base_name(nm), species_points(sp), typ{et[0], et[1]}, category(cat),
      poss_abilities(abilities), height(h), weight(w), catch_rate(cr),
      gender_ratio(gr), aquire_base_point(abp), exp_acc_speed(eas),
      base_exp(bexp), id(idx)
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

pkm_info *get_pkm_info(int id) { return &pkm_info_maper[id]; }

float get_nature_rate(int nature) { return 1.0; }

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
    }
}
std::vector<int> get_pkm_init_skills(const pkm_base &p, int level){
    pkm_info *pi = get_pkm_info(p.id);
    std::vector<int> ret;
    for(int i=pi->skills_can_learn.size()-1;i>=0;i--){
        if(pi->skills_can_learn[i].first <= level){
            ret.push_back(pi->skills_can_learn[i].second);
            if(ret.size() == 4){
                return ret;
            }
        }
    }
    return ret;
}

pkm::pkm(const pkm_base &pb, const std::string &namex, gender gen,
         u_char levelx, int exp_needx, int exp_currx, base6 IVx, base6 bp,
         u_char fship, battle_status bs, int skillx[4], int naturex, int hpred,
         bool is_sh)
    : pkm_base(pb), name(namex), gend(gen), level(levelx), exp_need(exp_needx),
      exp_curr(exp_currx), IV(IVx), base_points(bp), friendship(fship),
      bstatus(bs), nature(naturex), hpreduced(hpred), is_shiny(is_sh)
{
    refresh_stat();
    this->skills[0] = skillx[0];
    this->skills[1] = skillx[1];
    this->skills[2] = skillx[2];
    this->skills[3] = skillx[3];
}

pkm::pkm(const pkm_base &pb, const std::string &namex, gender gen,
         u_char levelx, int exp_needx, int exp_currx, base6 IVx, base6 bp,
         u_char fship, battle_status bs, std::array<int, 4> skillx, int naturex,
         int hpred, bool is_sh)
    : pkm_base(pb), name(namex), gend(gen), level(levelx), exp_need(exp_needx),
      exp_curr(exp_currx), IV(IVx), base_points(bp), friendship(fship),
      bstatus(bs), nature(naturex), hpreduced(hpred), is_shiny(is_sh)
{
    refresh_stat();
    this->skills[0] = skillx[0];
    this->skills[1] = skillx[1];
    this->skills[2] = skillx[2];
    this->skills[3] = skillx[3];
}

void pkm::refresh_stat()
{
    stat.hp =
        (int)((species_points.hp * 2 + IV.hp + (float)base_points.hp / 4) *
                  level / 100.0 +
              10 + level);

#define gst(a)                                                                 \
    (int)(((species_points.a * 2 + IV.a + (float)base_points.a / 4) * level /  \
               100.0 +                                                         \
           5) *                                                                \
          get_nature_rate(nature))
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

pkm pkm::create_pkm(const pkm_base &p, const std::string &nm, bool gen,
                    u_char level)
{
    auto skills = get_pkm_init_skills(p, level);
    while(skills.size() < 4) skills.push_back(0);
    return pkm(p, nm, static_cast<gender>(gen), level, get_next_level_exp(p.exp_acc_speed, level), 0, random_base6(32), (base6){0, 0, 0, 0, 0, 0}, 70, battle_status::NORMAL,
    {skills[0], skills[1], skills[2], skills[3]}, get_random(25), 0, get_random(4096) == 0);
}