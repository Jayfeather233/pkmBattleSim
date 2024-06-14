#pragma once

#include "basic.hpp"

#include <map>
#include <string>

struct pkm_info {
    std::string name;
    int id;
    u_char *image;
    std::vector<int> skills_can_learn;
};

extern std::vector<pkm_info> pkm_info_maper;
pkm_info *get_pkm_info(int id) { return &pkm_info_maper[id]; }

class pkm_base {
public:
    base6
        species_points; // 种族值是反映不同种类宝可梦之间各项能力大致情况的数值
    element_types typ[2];
    int id;
};

extern std::vector<pkm_base> pkm_list;

float get_nature_rate(int nature) { return 1.0; }

class pkm : public pkm_base {
public:
    std::string name;
    u_char gender;
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

    void refresh_stat()
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
};