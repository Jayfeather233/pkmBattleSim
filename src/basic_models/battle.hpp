#pragma once

#include "menu.hpp"

#include "skills.hpp"

struct move_struct {
    pkm *from;
    int skill_id; // skills_list[from->skills[skill_id]]
    int to;       // | (1<<battle_position) |
    // use item
    int exchange_pkm;
};

class battle_main {
public:
    player *p1, *p2;
    std::vector<pkm *> p1pkm;
    std::vector<pkm *> p2pkm;
    int weather;
    std::vector<move_struct> moves; // moves in one turn

    void pkm_entry_proc(std::vector<pkm*> pkms){
        // 化学变化气体特性()
        // 紧张感和人马一体特性()
        // other abilities
        // items
        // 花之礼、阴晴不定特性()
        // 若上一回合为宝可梦极巨化的第3回合，极巨化结束()
    }
    void exchange_pkm(){
        for(auto ms:moves){
            if(skill_list[ms.from->skills[ms.skill_id]].name == "追打"){
                
            }
        }
    }
    int process_turn()
    {
        exchange_pkm();
/*
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
*/
/*
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
        // process for one turn, 0: continue, 1: p1 win, -1: p2 win 2: escape

    }
};

extern text_menu *battle_menu;

void battle_start(player *p1, player *p2, int battle_num, int weather = 0)
{
    std::vector<pkm *> p1pkm, p2pkm;
    for (int i = 0; i < battle_num && i < p1->party_pkm.size(); i++) {
        p1pkm.push_back(&(p1->party_pkm[i]));
    }
    for (int i = 0; i < battle_num && i < p2->party_pkm.size(); i++) {
        p1pkm.push_back(&(p2->party_pkm[i]));
    }
    battle_main bm = (battle_main){p1, p2, p1pkm, p2pkm, weather, {}};
    int u;

    // battle text something
    while ((u = bm.process_turn()) == 0){
        get_next_battle_move(p1, battle_menu, &bm);
        get_next_battle_move(p2, battle_menu, &bm);
    }
    // end text something
}

void get_next_battle_move(player *p, text_menu *battle_menu, battle_main *bm){

}