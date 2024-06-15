#include "pkm.hpp"

pkm_base::pkm_base() { id = 0; }

pkm_base::pkm_base(base6 sp, std::array<element_types, 2> et, int idx)
    : species_points(sp), id(idx)
{
    typ[0] = et[0];
    typ[1] = et[1];
}
pkm_base::pkm_base(base6 sp, element_types et[2], int idx)
    : species_points(sp), id(idx)
{
    typ[0] = et[0];
    typ[1] = et[1];
}
pkm_base::pkm_base(const pkm_base &u)
    : species_points(u.species_points), id(u.id)
{
    typ[0] = u.typ[0];
    typ[1] = u.typ[1];
}
pkm_base::pkm_base(const pkm_base &&u)
    : species_points(u.species_points), id(u.id)
{
    typ[0] = u.typ[0];
    typ[1] = u.typ[1];
}

pkm_info *get_pkm_info(int id) { return &pkm_info_maper[id]; }

float get_nature_rate(int nature) { return 1.0; }

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