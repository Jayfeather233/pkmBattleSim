#pragma once

#include "basic.hpp"

#include <map>
#include <string>

struct pkm_info {
    std::string name;
    int id;
    u_char *pkm_image;
    std::vector<int> skills_can_learn;
};

extern std::vector<pkm_info> pkm_info_maper;
pkm_info *get_pkm_info(int id);

class pkm_base {
public:
    base6
        species_points; // 种族值是反映不同种类宝可梦之间各项能力大致情况的数值
    element_types typ[2];
    int id;

    pkm_base();

    pkm_base(base6 sp, std::array<element_types, 2> et, int idx);
    pkm_base(base6 sp, element_types et[2], int idx);
    pkm_base(const pkm_base &u);
    pkm_base(const pkm_base &&u);
};

extern std::vector<pkm_base> pkm_list;

float get_nature_rate(int nature);

class pkm : public pkm_base {
public:
    std::string name;
    gender gend;
    u_char level;
    int exp_need; // need to grant level
    int exp_curr; // current exp
    base6
        IV; // individual value, random generate at catch
            // 个体值是参与决定每只宝可梦能力的一组数值。个体值与生俱来，无法改变，是每只宝可梦的一个隐藏参数。
    base6
        base_points; // aquired throuth battle
                     // 基础点数是击败宝可梦后，除了经验值外可以获得的另一项数值
    u_char friendship;
    battle_status bstatus; // status aquired in battle (like been poison)
    int skills[4];         // skill_list[skills]
    int nature; // 性格于第三世代引入的宝可梦的一种特征。一共存在25种性格。
    base6 stat;
    int hpreduced;
    bool is_shiny;
    // battle_item

    struct battle_st {
        base6 affected; // like been reduced defence
        char affected_hitrate;
        char affected_evasionrate;
    } bstate;

    pkm(const pkm_base &pb, const std::string &namex, gender gen, u_char levelx,
        int exp_needx, int exp_currx, base6 IVx, base6 bp, u_char fship,
        battle_status bs, int skillx[4], int naturex, int hpred, bool is_sh);

    pkm(const pkm_base &pb, const std::string &namex, gender gen, u_char levelx,
        int exp_needx, int exp_currx, base6 IVx, base6 bp, u_char fship,
        battle_status bs, std::array<int, 4> skillx, int naturex, int hpred,
        bool is_sh);

    void refresh_stat();
    void refresh_bstate();
};