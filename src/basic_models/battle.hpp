#pragma once

#include "menu.hpp"

#include "skills.hpp"

struct move_struct {
    pkm *from;
    int from_pos;
    int from_side;
    int skill_id; // skills_list[from->skills[skill_id]]
    int to;       // selected one
    // use item

    int exchange_pkm; // exchange to player.party_pkm[exchange_pkm]
};

class battle_main {
public:
    player *p[2];
    std::vector<pkm *> pkms[2];
    std::vector<move_struct> moves; // moves in one turn

    field_status field[3]; // 0/1 player field, 2 overall field
    weather_status weather;

    enum sort_method { BY_PKM_SPEED, BY_MOVES };

    void sort_moves(sort_method method)
    {
        // TODO
    }

    std::vector<pkm *> aff_pkms(const move_struct &ms)
    {
        std::vector<pkm *> ret;
        int tgt = skill_list[ms.from->skills[ms.skill_id]].tgt;
        if (tgt & (1 << move_target::SPECIFIC)) {
            ret.push_back(
                pkms[ms.from_side ^ 1][ms.from->bstate.hb.las_hit_id]);
        }
        else if (tgt & (1 << move_target::ANY_OPPO)) {
            ret.push_back(pkms[ms.from_side ^ 1][ms.to]);
        }
        else if (tgt & (1 << move_target::ANY_ALLY)) {
            ret.push_back(pkms[ms.from_side][ms.to]);
        }
        else if (tgt & (1 << move_target::UESR_OR_AROUND_ALLY)) {
            ret.push_back(pkms[ms.from_side][ms.to]);
        }
        else if (tgt & (1 << move_target::USER)) {
            ret.push_back(ms.from);
        }
        else if (tgt & (1 << move_target::RANDOM_OPPO)) {
            ret.push_back(pkms[ms.from_side ^ 1]
                              [get_random(pkms[ms.from_side ^ 1].size())]);
        }
        else if (tgt & (1 << move_target::ALL_OPPO)) {
            for (auto it : pkms[ms.from_side ^ 1])
                ret.push_back(it);
        }
        else if (tgt & (1 << move_target::SELECTED_OPPO)) {
            ret.push_back(pkms[ms.from_side ^ 1][ms.to]);
        }
        else if (tgt & (1 << move_target::USER_AND_ALL_ALLY)) {
            for (auto it : pkms[ms.from_side])
                ret.push_back(it);
        }
        else if (tgt & (1 << move_target::ALL_ALLY)) {
            for (auto it : pkms[ms.from_side])
                if (it != ms.from)
                    ret.push_back(it);
        }
        else if (tgt & (1 << move_target::ALL)) {
            for (auto it : pkms[ms.from_side])
                ret.push_back(it);
            for (auto it : pkms[ms.from_side ^ 1])
                ret.push_back(it);
        }
        else if (tgt & (1 << move_target::ALL_FAINT)) {
            for (auto it : pkms[ms.from_side])
                if (it->hpreduced >= it->stat.hp)
                    ret.push_back(it);
            for (auto it : pkms[ms.from_side ^ 1])
                if (it->hpreduced >= it->stat.hp)
                    ret.push_back(it);
        }
        else {
            throw "skill error: no such target";
        }
    }

    void pkm_entry_proc(int side, int id, pkm *pkm)
    {
        // 化学变化气体特性()
        // 紧张感和人马一体特性()
        // other abilities
        // items
        // 花之礼、阴晴不定特性()
        // 若上一回合为宝可梦极巨化的第3回合，极巨化结束()
    }
    void execute_move(const move_struct &ms) {}
    void execute_change_pkm(const move_struct &ms)
    {
        // 隐形岩、毒菱、撒菱等场地型状态变化造成伤害
        pkm_entry_proc(ms.from_side, ms.from_pos,
                       &(p[ms.from_side]->party_pkm[ms.exchange_pkm]));
    }
    void exchange_pkm()
    {
        sort_moves(sort_method::BY_PKM_SPEED);
        std::set<pkm *> sub_list;
        for (auto ms : moves) {
            if (ms.exchange_pkm != -1) {
                sub_list.insert(ms.from);
            }
        }
        for (auto ms : moves) {
            if (skill_list[ms.from->skills[ms.skill_id]].uid ==
                228) { // Pursuit 追打
                if (sub_list.find(pkms[ms.from_side ^ 1][ms.to]) !=
                    sub_list.end()) {
                    execute_move(ms);
                }
            }
        }
        for (auto ms : moves) {
            if (ms.exchange_pkm != -1) {
                execute_change_pkm(ms);
            }
        }
    }
    void use_items()
    {
        for (auto it : moves) {
            // TODO
        }
    }
    void use_moves()
    {
        sort_moves(sort_method::BY_MOVES);
        /*
        按以下顺序使用招式：
        先按招式优先度修正后的顺序：优先度高的招式→先制之爪、释陀果、速击的发动者使用的招式→普通招式→后攻之尾、饱腹薰香、慢出的发动者使用的招式→优先度低的招式
        [1]当招式优先度相同时，按宝可梦从快到慢行动；若场上处于戏法空间状态，则按速度从慢到快行动
        一个招式使用结束后：招式负面效果发动→逃脱按键发动→和后备宝可梦进行替换的招式发动替换效果→进入愤怒状态
        */
    }
    void end_turn_proc()
    {
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
        */
    }
    int check_status() {}
    int process_turn()
    {
        exchange_pkm();
        use_items();

        /*
        训练家让宝可梦超级进化*：
        按进化前的速度由快至慢进行超级进化，若场上处于戏法空间状态，则反之
        按进化后速度由快至慢发动宝可梦入场时发动的特性
        训练家让宝可梦极巨化*：
        按速度由快至慢进行极巨化，若场上处于戏法空间状态，则反之
        训练家让宝可梦太晶化*：
        按速度由快至慢进行太晶化，若场上处于戏法空间状态，则反之
        */
        use_moves();
        end_turn_proc();

        return check_status();
        // process for one turn, 0: continue, 1: p1 win, -1: p2 win 2: escape
    }
};

extern text_menu *battle_menu;

// battle_num: n vs n
void battle_start(player *p1, player *p2, int battle_num,
                  weather_status weather = weather_status::CLEAR)
{
    std::vector<pkm *> p1pkm, p2pkm;
    for (int i = 0; i < battle_num && i < p1->party_pkm.size(); i++) {
        p1pkm.push_back(&(p1->party_pkm[i]));
    }
    for (int i = 0; i < battle_num && i < p2->party_pkm.size(); i++) {
        p1pkm.push_back(&(p2->party_pkm[i]));
    }
    battle_main bm =
        (battle_main){{p1, p2},
                      {p1pkm, p2pkm},
                      {},
                      {field_status::NO_FIELD, field_status::NO_FIELD,
                       field_status::NO_FIELD},
                      weather};
    int u;

    // battle text something
    while ((u = bm.process_turn()) == 0) {
        get_next_battle_move(0, &bm);
        get_next_battle_move(1, &bm);
    }
    // end text something
}

void get_next_battle_move(int side, battle_main *bm) {
    if(bm->p[side]->badge < 0){
        for(int i=0;i<bm->pkms[side].size();i++){
            int ux = 4;
            if(bm->pkms[side][i]->skills[3] != 0){ux = 4;}
            else if(bm->pkms[side][i]->skills[2] != 0){ux = 3;}
            else if(bm->pkms[side][i]->skills[1] != 0){ux = 2;}
            else if(bm->pkms[side][i]->skills[0] != 0){ux = 1;}
            else {
                throw "PKM has No skill";
            }

            bm->moves.push_back((move_struct){bm->pkms[side][i], i, side, get_random(ux), i, -1});
        }
    } else {
        for(auto u:bm->pkms[side]){
            for(int i=0;i<6;i++){
                if(&(bm->p[side]->party_pkm[i]) == u){
                    bm->p[side]->mt.menu_choose_pokemon = i;
                    run_text_menu(*(bm->p[side]), battle_menu, nullptr, root_menu);
                    // bm->moves.push_back((move_struct){bm->pkms[side][i], i, side, get_random(ux), i, -1});
                }
            }
        }
    }
}