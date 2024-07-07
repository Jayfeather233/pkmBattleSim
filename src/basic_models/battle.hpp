#pragma once

/*
    For the basic `turn`(回合), please refer to:
    https://wiki.52poke.com/wiki/%E5%9B%9E%E5%90%88
*/

#include "menu.hpp"
#include "skills.hpp"

#include <algorithm>

struct move_struct {
    pkm *from;
    int from_pos;
    int from_side;
    int skill_id;     // skills_list[from->skills[skill_id]]
    int to;           // selected one
    int mv_piror_fix; // like: 先制之爪+1，后攻之尾-1
    // use item

    int exchange_pkm; // exchange to player.party_pkm[exchange_pkm]
};

inline bool ms_cmp_pkm_slow2fast(const move_struct &a, const move_struct &b)
{
    return a.from->stat.spd == b.from->stat.spd
               ? get_random(2)
               : a.from->stat.spd < b.from->stat.spd;
}
inline bool ms_cmp_pkm_fast2slow(const move_struct &a, const move_struct &b)
{
    return ms_cmp_pkm_slow2fast(b, a);
}
inline bool ms_cmp_move_slow2fast(const move_struct &a, const move_struct &b)
{
    int u1 =
        (skill_list[a.from->skills[a.skill_id]].prior << 1) + a.mv_piror_fix;
    int u2 =
        (skill_list[b.from->skills[b.skill_id]].prior << 1) + b.mv_piror_fix;
    return u1 == u2 ? (a.from->stat.spd == b.from->stat.spd
                           ? get_random(2)
                           : a.from->stat.spd < b.from->stat.spd)
                    : u1 < u2;
}
inline bool ms_cmp_move_fast2slow(const move_struct &a, const move_struct &b)
{
    return ms_cmp_move_slow2fast(b, a);
}

struct pkm_list_item {
    pkm *from;
    int from_pos;
    int from_side;
    pkm *to;
};

inline bool pli_cmp_slow2fast(const pkm_list_item &a, const pkm_list_item &b)
{
    return a.from->stat.spd < b.from->stat.spd;
}
inline bool pli_cmp_fast2slow(const pkm_list_item &a, const pkm_list_item &b)
{
    return !(pli_cmp_slow2fast(a, b));
}

class battle_main {
public:
    bool is_escape;
    unsigned int escape_times;
    player *p[2];
    std::vector<pkm *> pkms[2];
    std::vector<move_struct> moves; // moves in one turn

    field_status field[3]; // 0/1 player field, 2 overall field
    weather_status weather;

    enum sort_method { BY_PKM_SPEED, BY_MOVES };

    void sort_moves(sort_method method)
    {
        std::sort(moves.begin(), moves.end(),
                  method == sort_method::BY_PKM_SPEED
                      ? (field[2] == field_status::TRICK_ROOM
                             ? ms_cmp_pkm_slow2fast
                             : ms_cmp_pkm_fast2slow)
                      : (field[2] == field_status::TRICK_ROOM
                             ? ms_cmp_move_slow2fast
                             : ms_cmp_move_fast2slow));
    }

    std::vector<std::pair<int, int>> aff_pkms(const move_struct &ms)
    {
        std::vector<std::pair<int, int>> ret;
        int tgt = skill_list[ms.from->skills[ms.skill_id]].tgt;
        if (tgt & (1 << move_target::SPECIFIC)) {
            ret.push_back(std::make_pair(ms.from_side ^ 1,
                                         ms.from->bstate.hb.las_hit_id));
        }
        else if (tgt & (1 << move_target::ANY_OPPO)) {
            ret.push_back(std::make_pair(ms.from_side ^ 1, ms.to));
        }
        else if (tgt & (1 << move_target::ANY_ALLY)) {
            ret.push_back(std::make_pair(ms.from_side, ms.to));
        }
        else if (tgt & (1 << move_target::UESR_OR_AROUND_ALLY)) {
            ret.push_back(std::make_pair(ms.from_side, ms.to));
        }
        else if (tgt & (1 << move_target::USER)) {
            ret.push_back(std::make_pair(ms.from_side, ms.from_pos));
        }
        else if (tgt & (1 << move_target::RANDOM_OPPO)) {
            ret.push_back(std::make_pair(
                ms.from_side ^ 1, get_random(pkms[ms.from_side ^ 1].size())));
        }
        else if (tgt & (1 << move_target::ALL_OPPO)) {
            for (size_t j = 0; j < pkms[ms.from_side ^ 1].size(); ++j)
                if (pkms[ms.from_side ^ 1][j] != nullptr)
                    ret.push_back(std::make_pair(ms.from_side ^ 1, j));
        }
        else if (tgt & (1 << move_target::SELECTED_OPPO)) {
            ret.push_back(std::make_pair(ms.from_side ^ 1, ms.to));
        }
        else if (tgt & (1 << move_target::USER_AND_ALL_ALLY)) {
            for (size_t j = 0; j < pkms[ms.from_side].size(); ++j)
                if (pkms[ms.from_side][j] != nullptr)
                    ret.push_back(std::make_pair(ms.from_side, j));
        }
        else if (tgt & (1 << move_target::ALL_ALLY)) {
            for (size_t j = 0; j < pkms[ms.from_side].size(); ++j)
                if (pkms[ms.from_side][j] != nullptr && ms.from_pos != j)
                    ret.push_back(std::make_pair(ms.from_side, j));
        }
        else if (tgt & (1 << move_target::ALL)) {
            for (size_t j = 0; j < pkms[ms.from_side ^ 1].size(); ++j)
                if (pkms[ms.from_side ^ 1][j] != nullptr)
                    ret.push_back(std::make_pair(ms.from_side ^ 1, j));
            for (size_t j = 0; j < pkms[ms.from_side].size(); ++j)
                if (pkms[ms.from_side][j] != nullptr)
                    ret.push_back(std::make_pair(ms.from_side, j));
        }
        else if (tgt & (1 << move_target::ALL_FAINT)) {
            for (size_t j = 0; j < pkms[ms.from_side ^ 1].size(); ++j)
                if (pkms[ms.from_side ^ 1][j] != nullptr &&
                    pkms[ms.from_side ^ 1][j]->hpreduced >=
                        pkms[ms.from_side ^ 1][j]->stat.hp)
                    ret.push_back(std::make_pair(ms.from_side ^ 1, j));
            for (size_t j = 0; j < pkms[ms.from_side].size(); ++j)
                if (pkms[ms.from_side][j] != nullptr &&
                    pkms[ms.from_side][j]->hpreduced >=
                        pkms[ms.from_side][j]->stat.hp)
                    ret.push_back(std::make_pair(ms.from_side, j));
        }
        else {
            throw "skill error: no such target";
        }

        // TODO: deal with nullptr

        return ret;
    }

    void set_prior_fix()
    {
        for (move_struct &ms : moves) {
            if (ms.from->carried_item == 194) { // quick_claw 先制之爪
                ms.mv_piror_fix = get_random(100) < 20 ? 1 : 0;
            }
        }
        // TODO
    }

    void pkm_entry_proc(int side, int id, pkm *pkm, bool is_first_entry = false)
    {
        // 化学变化气体特性()
        // 紧张感和人马一体特性()
        // other abilities
        // items
        // 花之礼、阴晴不定特性()
        // 若上一回合为宝可梦极巨化的第3回合，极巨化结束()
    }
    void execute_move(const move_struct &ms) {}
    void execute_change_pkm(const pkm_list_item &ms)
    {
        pkms[ms.from_side][ms.from_pos] = ms.to;
        // 隐形岩、毒菱、撒菱等场地型状态变化造成伤害
        pkm_entry_proc(ms.from_side, ms.from_pos, ms.to, true);
    }
    void exchange_pkm()
    {
        sort_moves(sort_method::BY_PKM_SPEED);
        std::vector<pkm_list_item> pls;
        for (int i = 0; i < 2; ++i) {
            for (size_t j = 0; j < pkms[i].size(); ++j) {
                if (pkms[i][j] != nullptr)
                    pls.push_back((pkm_list_item){pkms[i][j], i, j, nullptr});
            }
        }
        for (auto ms : moves) {
            if (ms.exchange_pkm != -1) {
                for (size_t i = 0; i < pls.size(); ++i) {
                    if (pls[i].from == ms.from) {
                        pls[i].to =
                            &(p[ms.from_side]->party_pkm[ms.exchange_pkm]);
                    }
                }
            }
        }
        std::sort(pls.begin(), pls.end(),
                  field[2] == field_status::TRICK_ROOM ? pli_cmp_slow2fast
                                                       : pli_cmp_fast2slow);
        for (pkm_list_item &u : pls) {
            for (auto ms : moves) {
                if (skill_list[ms.from->skills[ms.skill_id]].uid == 228 &&
                    ms.from->hpreduced < ms.from->stat.hp) { // Pursuit 追打
                    if (pkms[ms.from_side ^ 1][ms.to] == u.from &&
                        u.to != nullptr) {
                        execute_move(ms);
                    }
                }
            }
        }
        for (pkm_list_item &u : pls) {
            if (u.to != nullptr) {
                execute_change_pkm(u);
            }
            else {
                pkm_entry_proc(u.from_side, u.from_pos, u.from, false);
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
        for (int sd = 0; sd < 2; ++sd) {
            for (size_t i = 0; i < pkms[sd].size(); ++i) {
                if (pkms[sd][i] == nullptr)
                    continue;
                if (pkms[sd][i]->stat.hp <= pkms[sd][i]->hpreduced) {
                    int pid = p[sd]->get_subsitute_pkm(pkms[sd][i]);
                    if (pid >= 0 && pid < p[sd]->party_pkm.size()) {
                        pkm *subs = &(p[sd]->party_pkm[pid]);
                        if (subs->stat.hp <= subs->hpreduced) {
                            throw "subsitute pkm is fainting.";
                        }
                        pkms[sd][i] = subs;
                        // TODO: text here
                    }
                    else if (pid < 0) {
                        pkms[sd][i] = nullptr;
                    }
                    else {
                        throw "subsitute pkm id OutOfBound.";
                    }
                }
            }
        }
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
    int check_status()
    {
        for (int i = 0; i < 2; ++i) {
            bool flg = true;
            for (size_t j = 0; j < pkms[i].size(); ++j) {
                if (pkms[i][j] != nullptr) {
                    flg = false;
                }
            }
            if (flg == true) {
                return (i << 1) - 1;
            }
        }
        return 0;
    }
    bool escape_detec()
    {
        if (!is_escape)
            return false;
        is_escape = false;
        escape_times += 1;
        if (p[1]->badge != -1) {
            // TODO: text here, cannot escape fron a trainer
            return false;
        }
        else {
            /*
            携带烟雾球或拥有逃跑特性的宝可梦逃走必定成功，无论对方的特性是什么以及自己处于什么状态。
            在第三世代，使用烟雾球逃走有特别的动画效果。
            使用宝可尾草、向尾喵的尾巴、皮皮玩偶等道具，无论对方的特性是什么以及自己处于什么状态。
            在没有被限制逃走的效果影响时，使用瞬间移动可以逃走成功。
            */
            float avg_spd[2] = {0, 0};
            for (int i = 0; i < 2; ++i) {
                int cnt = 0;
                for (size_t j = 0; j < pkms[i].size(); ++j) {
                    if (pkms[i][j] != nullptr) {
                        avg_spd[i] += pkms[i][j]->stat.spd;
                        ++cnt;
                    }
                }
                avg_spd[i] /= cnt;
            }
            int F = (avg_spd[0] * 32) / avg_spd[1] * 4 + 30 * escape_times;
            for (size_t j = 0; j < pkms[0].size(); ++j) {
                if (pkms[0][j]->typ[0] == element_types::GHOST ||
                    pkms[0][j]->typ[1] == element_types::GHOST) {
                    F = 256.0;
                }
            }
            int D = get_random(256);
            if (D < F) {
                // TODO: test here
                return true;
            }
            else {
                // TODO: text here
                return false;
            }
        }
    }
    int process_turn()
    {
        if (escape_detec()) {
            return 2;
        }
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
        set_prior_fix();
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
        (battle_main){false,
                      0,
                      {p1, p2},
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

void get_next_battle_move(int side, battle_main *bm)
{
    if (bm->p[side]->badge < 0) {
        for (int i = 0; i < bm->pkms[side].size(); i++) {
            auto u = bm->pkms[side][i];
            if (u == nullptr)
                continue;
            int ux = 4;
            if (u->skills[3] != 0) {
                ux = 4;
            }
            else if (u->skills[2] != 0) {
                ux = 3;
            }
            else if (u->skills[1] != 0) {
                ux = 2;
            }
            else if (u->skills[0] != 0) {
                ux = 1;
            }
            else {
                throw "PKM has No skill";
            }

            bm->moves.push_back(
                (move_struct){u, i, side, get_random(ux), i, -1});
        }
    }
    else {
        for (int j = 0; j < bm->pkms[side].size(); ++j) {
            auto u = bm->pkms[side][j];
            if (u == nullptr)
                continue;
            for (int i = 0; i < 6; i++) {
                if (&(bm->p[side]->party_pkm[i]) == u) {
                    bm->p[side]->mt.menu_choose_pokemon = i;
                    run_text_menu(*(bm->p[side]), battle_menu, nullptr,
                                  root_menu);
                    if (bm->p[side]->mt.battle_change_pkm != -1) {
                        bm->moves.push_back(
                            (move_struct){u, j, side, 0, 0,
                                          bm->p[side]->mt.battle_change_pkm});
                    }
                    else {
                        bm->moves.push_back((move_struct){
                            u, j, side, bm->p[side]->mt.menu_choose_id,
                            bm->p[side]->mt.menu_choose_position, -1});
                    }
                }
            }
        }
    }
}