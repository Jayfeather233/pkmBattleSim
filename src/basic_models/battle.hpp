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

    int use_item;
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

// for pkm exchange
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
    int battle_num;
    bool is_escape;
    unsigned int escape_times;
    player *p[2];
    std::vector<pkm *> pkms[2];
    std::vector<std::set<pkm *>> seened_pkms[2];
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

    inline bool check_valid_pos(int side, int id)
    {
        return side < 2 && 0 <= side && 0 <= id && id < pkms[side].size();
    }
    inline bool check_valid_pkm(int side, int id)
    {
        return check_valid_pos(side, id) &&
               pkms[side][id] != nullptr;
    }

    std::vector<std::pair<int, int>> aff_pkms(const move_struct &ms)
    {
        std::vector<std::pair<int, int>> ret;
        move_target tgt = skill_list[ms.from->skills[ms.skill_id]].tgt;
        if (tgt == move_target::SPECIFIC) {
            ret.push_back(std::make_pair(ms.from_side ^ 1,
                                         ms.from->bstate6.hb.las_hit_id));
            // TODO: special or physical
        }
        else if (tgt == move_target::AROUND_USER) {
            int msto = ms.to;
            if(ms.from_pos == 0){
                msto += 2;
            }
            if(1 <= msto && msto <= 3){
                int l1 = ms.from_side ^ 1;
                int l2 = ms.from_pos + msto - 2;
                if(!check_valid_pkm(l1, l2)){
                    l2 = ms.from_pos;
                    int off = -1;
                    while (!check_valid_pkm(l1, l2)) {
                        l2 += off;
                        off = (off < 0 ? -off + 1 : -off - 1);
                        if (off == 4) {
                            l2 = ms.from_pos + msto - 2;
                            break;
                        }
                    }
                }
                ret.push_back(std::make_pair(l1, l2));
            } else {
                ret.push_back(std::make_pair(ms.from_side, ms.to == 0 ? ms.from_pos - 1 : ms.from_pos + 1));
            }
        }
        else if (tgt == move_target::USER) {
            ret.push_back(std::make_pair(ms.from_side, ms.from_pos));
        }
        else if (tgt == move_target::ALL_OPPO) {
            for (size_t j = 0; j < pkms[ms.from_side ^ 1].size(); ++j)
                if (pkms[ms.from_side ^ 1][j] != nullptr)
                    ret.push_back(std::make_pair(ms.from_side ^ 1, j));
        }
        else if (tgt == move_target::ANY_OPPO) {
            int l1 = ms.from_side ^ 1;
            int l2 = ms.from_pos + ms.to - 1;
            if(!check_valid_pkm(l1, l2)){
                l2 = ms.from_pos;
                int off = -1;
                while (!check_valid_pkm(l1, l2)) {
                    l2 += off;
                    off = (off < 0 ? -off + 1 : -off - 1);
                    if (off == 4) {
                        l2 = ms.from_pos + ms.to - 1;
                        break;
                    }
                }
            }
            ret.push_back(std::make_pair(l1, l2));
        }
        else if (tgt == move_target::ANY_ALLY) {
            int l1 = ms.from_side;
            int l2 = (ms.to >= ms.from_pos ? ms.to + 1 : ms.to);
            int off = -1;
            while (!check_valid_pkm(l1, l2) || l2 == ms.from_pos) {
                l2 += off;
                off = (off < 0 ? -off + 1 : -off - 1);
                if (off > pkms[l1].size() + 2) {
                    l2 = ms.to;
                    break;
                }
            }
            ret.push_back(std::make_pair(l1, l2));
        }
        else if (tgt == move_target::UESR_OR_AROUND_ALLY) {
            int l1 = ms.from_side;
            int l2 = ms.from_pos + ms.to - 1;
            if (!check_valid_pkm(l1, l2)) {
                l2 = ms.from_pos;
                int off = -1;
                while (!check_valid_pkm(l1, l2)) {
                    l2 += off;
                    off = (off < 0 ? -off + 1 : -off - 1);
                    if (off == 4) {
                        l2 = ms.from_pos;
                        break;
                    }
                }
            }
            ret.push_back(std::make_pair(l1, l2));
        }
        else if (tgt == move_target::RANDOM_OPPO) {
            int l1 = ms.from_side ^ 1;
            int l2 = get_random(pkms[ms.from_side ^ 1].size());
            int off = -1;
            while (!check_valid_pkm(l1, l2)) {
                l2 += off;
                off = (off < 0 ? -off + 1 : -off - 1);
                if (off > pkms[l1].size() + 2) {
                    l2 = 0;
                    break;
                }
            }
            ret.push_back(std::make_pair(l1, l2));
        }
        else if (tgt == move_target::ALL_OPPO) {
            for (size_t j = 0; j < pkms[ms.from_side ^ 1].size(); ++j)
                if (pkms[ms.from_side ^ 1][j] != nullptr)
                    ret.push_back(std::make_pair(ms.from_side ^ 1, j));
        }
        else if (tgt == move_target::SELECTED_OPPO) {
            int l1 = ms.from_side ^ 1;
            int l2 = ms.to - 1 + ms.from_pos;
            if (!check_valid_pkm(l1, l2)) {
                l2 = ms.from_pos;
                int off = -1;
                while (!check_valid_pkm(l1, l2)) {
                    l2 += off;
                    off = (off < 0 ? -off + 1 : -off - 1);
                    if (off == 4) {
                        l2 = ms.from_pos;
                        break;
                    }
                }
            }
            ret.push_back(std::make_pair(l1, l2));
        }
        else if (tgt == move_target::USER_AND_ALL_ALLY) {
            for (size_t j = 0; j < pkms[ms.from_side].size(); ++j)
                if (pkms[ms.from_side][j] != nullptr)
                    ret.push_back(std::make_pair(ms.from_side, j));
        }
        else if (tgt == move_target::ALL_ALLY) {
            for (size_t j = 0; j < pkms[ms.from_side].size(); ++j)
                if (pkms[ms.from_side][j] != nullptr && ms.from_pos != j)
                    ret.push_back(std::make_pair(ms.from_side, j));
        }
        else if (tgt == move_target::ALL) {
            for (size_t j = 0; j < pkms[ms.from_side ^ 1].size(); ++j)
                if (pkms[ms.from_side ^ 1][j] != nullptr)
                    ret.push_back(std::make_pair(ms.from_side ^ 1, j));
            for (size_t j = 0; j < pkms[ms.from_side].size(); ++j)
                if (pkms[ms.from_side][j] != nullptr)
                    ret.push_back(std::make_pair(ms.from_side, j));
        }
        else {
            throw "skill error: no such target";
        }

        return ret;
    }

    void set_prior_fix()
    {
        for (move_struct &ms : moves) {
            if (ms.exchange_pkm == -1 && ms.use_item == -1) {
                if (ms.from->carried_item == 194) { // quick_claw 先制之爪
                    ms.mv_piror_fix = get_random(100) < 20 ? 1 : 0;
                }
            }
        }
        // TODO
    }

    void refresh_seened_pkm()
    {
        for (int i = 0; i < 2; i++) {
            for (auto it : seened_pkms[i]) {
                for (auto it2 = it.begin(); it2 != it.end();) {
                    if ((*it2) == nullptr || IS_FAINT(*it2)) {
                        it2 = it.erase(it2); // erase returns the iterator to
                                             // the next element
                    }
                    else {
                        ++it2; // only increment if no element is erased
                    }
                }
                // in c++20: std::erase_if()
            }
        }
    }

    /**
     * If the pkm[side][pos] is faint, get the exp gained
     */
    void get_gained_exp(int side, int pos)
    {
        refresh_seened_pkm();
        if (p[side ^ 1]->badge < 0)
            return;
        float a = (p[side]->badge == -1) ? 1 : 1.5;
        int b = pkms[side][pos]->base_exp;
        int l = pkms[side][pos]->level;
        std::set<pkm *> to_gain = seened_pkms[side][pos];
        for (auto it : p[side ^ 1]->party_pkm) {
            if (it.carried_item == -1) { // TODO: learning machine here
                to_gain.insert(&it);
            }
        }
        size_t s = to_gain.size();
        for (auto u : to_gain) {
            float e =
                (u->carried_item == -1 /* TODO: carried lucky egg */) ? 1.5 : 1;
            int lp = u->level;
            float f = (u->friendship >= 150 ? 1.2 : 1.0);
            size_t i;
            for (i = 0; i < p[side ^ 1]->party_pkm.size(); ++i) {
                if (&(p[side ^ 1]->party_pkm[i]) == u) {
                    break;
                }
            }
            if (i != p[side ^ 1]->party_pkm.size()) {
                p[side ^ 1]->gain_exp(
                    i, (a * b * l / (5.0 * s) *
                            std::pow((2.0 * l + 10) / (l + lp + 10.0), 2.5) +
                        1) *
                           e * f);
            }
        }
    }

    void pkm_entry_proc(int side, int id, pkm *pkm, bool is_first_entry = false)
    {
        if (is_first_entry) {
            for (auto it : seened_pkms[side ^ 1]) {
                it.insert(pkm);
            }
            seened_pkms[side][id].clear();
        }
        // 化学变化气体特性
        // 紧张感和人马一体特性
        // other abilities
        // items
        // 花之礼、阴晴不定特性
        // 若上一回合为宝可梦极巨化的第3回合，极巨化结束()
    }
    void execute_move(const move_struct &ms)
    {
        // TODO: text here
        if (!check_valid_pkm(ms.from_side, ms.from_pos))
            return;
        if (skill_list[ms.from->skills[ms.skill_id]].tgt == OPPO_FIELD) {
            skill_list[ms.from->skills[ms.skill_id]].affect(
                field[ms.from_side ^ 1]);
            return;
        }
        else if (skill_list[ms.from->skills[ms.skill_id]].tgt == USER_FIELD) {
            skill_list[ms.from->skills[ms.skill_id]].affect(
                field[ms.from_side]);
            return;
        }
        else if (skill_list[ms.from->skills[ms.skill_id]].tgt == ENTIRE_FIELD) {
            skill_list[ms.from->skills[ms.skill_id]].affect(field[2]);
            return;
        }
        auto afpkm = aff_pkms(ms);
        for (auto topkm : afpkm) {
            if (check_valid_pkm(topkm.first, topkm.second)) {
                auto ttpkm = pkms[topkm.first][topkm.second];
                bool is_cri = false;
                skill_list[ms.from->skills[ms.skill_id]].affect(*(ms.from),
                                                                *ttpkm, is_cri);
                if (IS_FAINT(ttpkm)) {
                    // TODO: faint text here
                    get_gained_exp(topkm.first, topkm.second);
                }
            }
        }
    }

    void field_effect(field_status fs, pkm *p)
    {
        // TODO text_here
        if (fs == field_status::SPIKES) {
            p->hpreduced += p->stat.hp / 8;
        }
        else if (fs == field_status::SPIKES2) {
            p->hpreduced += p->stat.hp / 4;
        }
        else if (fs == field_status::SPIKES3) {
            p->hpreduced += 3 * p->stat.hp / 8;
        }
        else if (fs == field_status::STICKY_WEB) {
            p->bstate6.affected.spd -= 1;
        }
        else if (fs == field_status::TOXIC_SPICKES) {
            if (p->bstatus == battle_status::NORMAL) {
                p->bstatus = battle_status::POISONED;
            }
        }
        else if (fs == field_status::TOXIC_SPICKES2) {
            if (p->bstatus == battle_status::NORMAL) {
                p->bstatus = battle_status::BADLY_POISONED;
            }
        }
        else if (fs == field_status::STEALTH_ROCK) {
            float eft = eff_table[element_types::ROCK][p->typ[0]] *
                        eff_table[element_types::ROCK][p->typ[1]];
            p->hpreduced += eft * p->stat.hp / 8;
        }
    }
    void execute_change_pkm(const pkm_list_item &ms)
    {
        // TODO: text here
        pkms[ms.from_side][ms.from_pos] = ms.to;
        ms.to->refresh_stat();
        ms.to->refresh_bstate6();
        field_effect(field[ms.from_side], ms.to);
        pkm_entry_proc(ms.from_side, ms.from_pos, ms.to, true);
    }
    void exchange_pkm(bool first_turn)
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
                if (ms.use_item == -1 && ms.exchange_pkm == -1 &&
                    skill_list[ms.from->skills[ms.skill_id]].uid == 228 &&
                    !IS_FAINT(ms.from)) { // Pursuit 追打
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
                pkm_entry_proc(u.from_side, u.from_pos, u.from, first_turn);
            }
        }
    }
    void use_items()
    {
        for (auto it : moves) {
            if (it.use_item != -1) {
                // player.items[it.use_item]
                // TODO
            }
        }
    }
    void use_moves()
    {
        sort_moves(sort_method::BY_MOVES);
        for (auto mv : moves) {
            if (mv.exchange_pkm == -1 && mv.use_item == -1) {
                execute_move(mv);
            }
        }
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
                if (IS_FAINT(pkms[sd][i])) {
                    int pid = p[sd]->get_subsitute_pkm(pkms[sd]);
                    if (pid >= 0 && pid < p[sd]->party_pkm.size()) {
                        pkm *subs = &(p[sd]->party_pkm[pid]);
                        if (IS_FAINT(subs)) {
                            throw "subsitute pkm is fainting.";
                        }
                        pkm_list_item pli =
                            (pkm_list_item){pkms[sd][i], sd, i, subs};
                        execute_change_pkm(pli);
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
        #
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
                // TODO: text here
                return true;
            }
            else {
                // TODO: text here
                return false;
            }
        }
    }
    int process_turn(bool first_turn)
    {
        if (escape_detec()) {
            return 2;
        }
        exchange_pkm(first_turn);
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
                (move_struct){u, i, side, get_random(ux), i, -1, -1});
        }
    }
    else {
        for (int j = 0; j < bm->pkms[side].size(); ++j) {
            auto u = bm->pkms[side][j];
            if (u == nullptr)
                continue;
            for (int i = 0; i < 6; i++) {
                if (&(bm->p[side]->party_pkm[i]) == u) {
                    map_finder((std::string) "reset_all_menu_tmp",
                               action_mapper)(*(bm->p[side]));
                    bm->p[side]->mt.menu_choose_pokemon = i;
                    run_text_menu(*(bm->p[side]), battle_menu, nullptr,
                                  root_menu);
                    if (bm->p[side]->mt.is_goback) {
                        i -= 2;
                        continue;
                    }
                    if (bm->p[side]->mt.battle_change_pkm != -1) {
                        bm->moves.push_back((move_struct){
                            u, j, side, bm->p[side]->mt.menu_choose_id,
                            bm->p[side]->mt.menu_choose_position, -1,
                            bm->p[side]->mt.battle_change_pkm});
                    }
                    else if (bm->p[side]->mt.menu_choose_item != -1) {
                        bm->moves.push_back((move_struct){
                            u, j, side, bm->p[side]->mt.menu_choose_id,
                            bm->p[side]->mt.menu_choose_position,
                            bm->p[side]->mt.menu_choose_item, -1});
                    }
                    else {
                        bm->moves.push_back((move_struct){
                            u, j, side, bm->p[side]->mt.menu_choose_id,
                            bm->p[side]->mt.menu_choose_position, -1, -1});
                    }
                    break;
                }
                if (i == 5) {
                    throw "The battle pkm is not your party pkm";
                }
            }
        }
    }
}

// battle_num: n vs n
int battle_start(player *p1, player *p2, int battle_num,
                 weather_status weather = weather_status::CLEAR)
{
    std::vector<pkm *> p1pkm, p2pkm;
    int cnt = 0;
    for (int i = 0; cnt < battle_num && i < p1->party_pkm.size(); i++) {
        if (!IS_FAINT(&(p1->party_pkm[i]))) {
            p1->party_pkm[i].refresh_stat();
            p1->party_pkm[i].refresh_bstate6();
            p1pkm.push_back(&(p1->party_pkm[i]));
            ++cnt;
        }
    }
    cnt = 0;
    for (int i = 0; cnt < battle_num && i < p2->party_pkm.size(); i++) {
        if (!IS_FAINT(&(p2->party_pkm[i]))) {
            p2->party_pkm[i].refresh_stat();
            p2->party_pkm[i].refresh_bstate6();
            p2pkm.push_back(&(p2->party_pkm[i]));
            ++cnt;
        }
    }
    while (p1pkm.size() < battle_num) {
        p1pkm.push_back(nullptr);
    }
    while (p2pkm.size() < battle_num) {
        p2pkm.push_back(nullptr);
    }
    battle_main bm =
        (battle_main){battle_num,
                      false,
                      0,
                      {p1, p2},
                      {p1pkm, p2pkm},
                      {},
                      {},
                      {field_status::NO_FIELD, field_status::NO_FIELD,
                       field_status::NO_FIELD},
                      weather};
    int u;

    p1->bm = &bm;
    p2->bm = &bm;

    // battle text something
    bool first_turn = true;
    while ((u = bm.process_turn(first_turn)) == 0) {
        get_next_battle_move(0, &bm);
        get_next_battle_move(1, &bm);
        first_turn = false;
    }
    // end text something
    return u;
}
