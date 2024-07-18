#pragma once

#include "basic.hpp"
#include "defs.hpp"

#include <map>
#include <string>

struct pkm_info {
    int id;
    image pkm_image;
    std::vector<std::pair<int, int>>
        skills_can_learn; // level - skill_id
    std::vector<int> skills_can_learn_bymachine;
    std::vector<std::pair<int, float> > item_poss_carry;
};

extern std::vector<pkm_info> pkm_info_maper;
pkm_info *get_pkm_info(int id);
int get_next_level_exp(u_char eas, u_char level);

class pkm_base {
public:
    std::string base_name;
    base6
        species_points; // 种族值是反映不同种类宝可梦之间各项能力大致情况的数值
    element_types typ[2];
    std::string category; // what-what pokemon
    std::vector<std::pair<int, float>>
        poss_abilities; // 可能的特性, id - possibility
    std::vector<std::pair<int, float>>
        poss_items; // 可能的物品, id - possibility

    float height, weight;
    u_char catch_rate;
    int gender_ratio; // male will be ratio/8, no gender will be -1
    // int monster_type; // 蛋群 well, just cancel this limit
    base6 aquire_base_point; // the base_point you get when defeat
    u_char
        exp_acc_speed; // https://wiki.52poke.com/wiki/%E7%BB%8F%E9%AA%8C%E5%80%BC
                       // 0-fastest 5-slowest
    int base_exp; // 基础经验值用于对战后经验值获得计算
    int id;

    // Default constructor
    pkm_base();

    // Parameterized constructor 1
    pkm_base(const std::string &nm, base6 sp, std::array<element_types, 2> et,
             int idx, const std::string &cat,
             const std::vector<std::pair<int, float>> &abilities, float h,
             float w, u_char cr, int gr, base6 abp, u_char eas, int bexp,
             std::vector<std::pair<int, float>> poss_itemx = {});

    // Parameterized constructor 2
    pkm_base(const std::string &nm, base6 sp, element_types et[2], int idx,
             const std::string &cat,
             const std::vector<std::pair<int, float>> &abilities, float h,
             float w, u_char cr, int gr, base6 abp, u_char eas, int bexp,
             std::vector<std::pair<int, float>> poss_itemx = {});

    // Copy constructor
    pkm_base(const pkm_base &u);

    // Move constructor
    pkm_base(pkm_base &&u);
};

extern std::vector<pkm_base> pkm_list;
extern std::vector<int> first_pkm_list;

float get_nature_rate(int nature);

class pkm : public pkm_base {
private:
    std::string name;

public:
    std::string get_name() const
    {
        return name.length() == 0 ? base_name : name;
    }
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
    size_t skills[4]; // skill_list[skills]
    int used_pp[4];
    int nature; // 性格于第三世代引入的宝可梦的一种特征。一共存在25种性格。
    bool is_shiny;
    int carried_item;
    int ability; // poss_abilities[ability]

    base6 stat; // calculated basic stat
    int hpreduced;
    battle_status bstatus; // status aquired in battle (like been poison)
    struct battle_st {
        base6 affected; // like been reduced defence
        char affected_hitrate;
        char affected_evasionrate;

        struct hitted_by{
            int las_hit_damage;
            int las_hit_id;
        } hb;
    } bstate;

    pkm(const pkm_base &pb, const std::string &namex, gender gen, u_char levelx,
        int exp_needx, int exp_currx, base6 IVx, base6 bp, u_char fship,
        battle_status bs, size_t skillx[4], int used_ppx[4], int naturex,
        int hpred, bool is_sh, int caritem, int abi);

    pkm(const pkm_base &pb, const std::string &namex, gender gen, u_char levelx,
        int exp_needx, int exp_currx, base6 IVx, base6 bp, u_char fship,
        battle_status bs, std::array<size_t, 4> skillx,
        std::array<int, 4> used_ppx, int naturex, int hpred, bool is_sh,
        int caritem, int abi);

    void refresh_stat();
    void refresh_bstate();

    static pkm create_pkm(const pkm_base &p, const std::string &nm, bool gen,
                          u_char level);
};