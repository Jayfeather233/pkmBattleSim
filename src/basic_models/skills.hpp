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
    virtual void affect(pkm &self, pkm &p) {}
    base_skill(const std::string nm, const std::string ds,
               const std::vector<std::string> hds, char ar, char hr)
        : name(nm), desc(ds), hitdesc(hds), affect_range(ar), hitrate(hr)
    {
    }
    base_skill(const base_skill &bs)
        : name(bs.name), desc(bs.desc), hitdesc(bs.hitdesc),
          affect_range(bs.affect_range), hitrate(bs.hitrate)
    {
    }
    base_skill(const base_skill &&bs)
        : name(bs.name), desc(bs.desc), hitdesc(bs.hitdesc),
          affect_range(bs.affect_range), hitrate(bs.hitrate)
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
    aff_move() : base_skill("", "", {}, 0, 0) {}
};

// just attack
class atk_move : public base_skill {
public:
    element_types tp;
    bool sp_or_norm; // false for physical attack
    u_char atk;
    bool is_easy_critical;

    struct side_effect {
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

    atk_move(const std::string nm, const std::string ds,
             const std::vector<std::string> hds, char ar, char hr,
             element_types et, bool son, u_char atkx, bool easy_cri,
             side_effect w)
        : base_skill(nm, ds, hds, ar, hr), tp(et), sp_or_norm(son), atk(atkx),
          is_easy_critical(easy_cri), se(w)
    {
    }
};

// predefined skills
extern std::vector<base_skill> skill_list;
void init_skills();

/*
双方宝可梦入场流程：
发动化学变化气体特性
发动紧张感和人马一体特性
按宝可梦速度发动如降雨、无关天气、威吓等宝可梦入场时发动的特性
按宝可梦速度使用树果等道具
按宝可梦速度发动花之礼、阴晴不定特性
若上一回合为宝可梦极巨化的第3回合，极巨化结束
回合开始
双方选择命令，之后按如下顺序执行命令：
按宝可梦速度交换宝可梦：
发动追打
交换宝可梦，宝可梦入场
隐形岩、毒菱、撒菱等场地型状态变化造成伤害
进行宝可梦入场流程
训练家使用道具
训练家让宝可梦超级进化*：
按进化前的速度由快至慢进行超级进化，若场上处于戏法空间状态，则反之
按进化后速度由快至慢发动宝可梦入场时发动的特性
训练家让宝可梦极巨化*：
按速度由快至慢进行极巨化，若场上处于戏法空间状态，则反之
训练家让宝可梦太晶化*：
按速度由快至慢进行太晶化，若场上处于戏法空间状态，则反之
按以下顺序使用招式：
先按招式优先度修正后的顺序：优先度高的招式→先制之爪、释陀果、速击的发动者使用的招式→普通招式→后攻之尾、饱腹薰香、慢出的发动者使用的招式→优先度低的招式
[1]当招式优先度相同时，按宝可梦从快到慢行动；若场上处于戏法空间状态，则按速度从慢到快行动
一个招式使用结束后：招式负面效果发动→逃脱按键发动→和后备宝可梦进行替换的招式发动替换效果→进入愤怒状态
回合末流程：
若天气型状态已达到持续回合，状态结束；若天气型状态持续：发动天气损失ＨＰ的效果→与天气有关的特性发动
预知未来／破灭之愿攻击
祈愿实现，回复ＨＰ
发动影响ＨＰ的道具、状态变化或异常状态
状态变化解除
发动其他回合末发动的特性或道具效果
若当回合有宝可梦陷入濒死，且训练家还有其它宝可梦，则选择后备宝可梦上场，并进入入场流程
在第三世代对战中，宝可梦在回合结束前击倒时，玩家要立即替换下一只宝可梦；从第四世代开始，必须等到回合结束后才能替换下一只宝可梦
回合结束
*/