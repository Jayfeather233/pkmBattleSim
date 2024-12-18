#include "myutils.hpp"
#include "pkm.hpp"
#include "texts.hpp"
#include <fmt/core.h>

Json::Value set2Ja(const std::set<std::string> &s)
{
    Json::Value Ja(Json::arrayValue);
    for (auto it : s) {
        Ja.append(it);
    }
    return Ja;
}
std::set<std::string> Ja2set(const Json::Value &Ja)
{
    std::set<std::string> ret;
    for (auto it : Ja) {
        ret.insert(it.asString());
    }
    return ret;
}

std::vector<int> Ja2Vec(const Json::Value &Ja)
{
    std::vector<int> ret;
    for (Json::Value it : Ja) {
        ret.push_back(it.asInt());
    }
    return ret;
}

std::vector<std::pair<int, int>> Ja2VecP(const Json::Value &Ja)
{
    std::vector<std::pair<int, int>> ret;
    for (Json::Value it : Ja) {
        ret.push_back(std::make_pair(it["level"].asInt(), it["skill"].asInt()));
    }
    return ret;
}
std::vector<std::pair<int, float>> Ja2VecPo(const Json::Value &Ja)
{
    std::vector<std::pair<int, float>> ret;
    for (Json::Value it : Ja) {
        ret.push_back(std::make_pair(it["id"].asInt(), it["poss"].asFloat()));
    }
    return ret;
}

std::vector<std::string> Ja2VecS(const Json::Value &Ja)
{
    std::vector<std::string> ret;
    for (Json::Value it : Ja) {
        ret.push_back(it.asString());
    }
    return ret;
}

std::vector<std::pair<int, float>> Ja2Vec2(const Json::Value &Ja)
{
    std::vector<std::pair<int, float>> ret;
    for (Json::Value it : Ja) {
        ret.push_back(std::make_pair(it["id"].asInt(), it["chance"].asFloat()));
    }
    return ret;
}

base6 J2base6(const Json::Value &J)
{
    return (base6){
        J[0].asInt(), J[1].asInt(), J[2].asInt(), J[3].asInt(), J[4].asInt(), J[5].asInt(),
    };
}

element_types J2et(const Json::Value &J)
{
    try {
        return static_cast<element_types>(J.asInt());
    }
    catch (...) {
        return str2et(J.asString());
    }
}

pkm_base J2pkm_base(const Json::Value &J)
{
    // Extract species_points
    base6 sp = J2base6(J["sp"]);

    // Extract element_types
    std::array<element_types, 2> et = {J2et(J["tp"][0]), J2et(J["tp"][1])};

    // Extract poss_abilities
    std::vector<std::pair<int, float>> abilities;
    for (const auto &ability : J["poss_abilities"]) {
        abilities.push_back(std::make_pair(ability["id"].asInt(), ability["poss"].asFloat()));
    }

    // Extract aquire_base_point
    base6 abp = J2base6(J["aquire_base_point"]);

    // Create and return the pkm_base object
    return pkm_base(J["name"].asString(), sp, et, J["id"].asInt(), J["category"].asString(), abilities,
                    J["height"].asFloat(), J["weight"].asFloat(), static_cast<u_char>(J["catch_rate"].asUInt()),
                    J["gender_ratio"].asInt(), abp, static_cast<u_char>(J["exp_acc_speed"].asUInt()),
                    J["base_exp"].asInt(), Ja2VecPo(J["poss_items"]));
}

pkm J2pkm(const Json::Value &J)
{
    return pkm(pkm_list[J["id"].asInt()], J["name"].asString(), J["get_time"].asString(), J["get_place"].asString(),
               static_cast<gender>(J["gender"].asInt()), J["level"].asInt(), J["exp_curr"].asInt(), J2base6(J["IV"]),
               J2base6(J["bp"]), J["friendship"].asInt(),
               {J["skill"][0].asUInt(), J["skill"][1].asUInt(), J["skill"][2].asUInt(), J["skill"][3].asUInt()},
               {J["used_pp"][0].asInt(), J["used_pp"][1].asInt(), J["used_pp"][2].asInt(), J["used_pp"][3].asInt()},
               J["nature"].asInt(), J["is_shiny"].asBool(), J["carry_item"].asInt(), J["ability"].asInt(),
               J["hpreduced"].asInt(), static_cast<battle_status>(J["battle_status"].asInt()));
}
std::vector<pkm> Ja2pkm(const Json::Value &Ja)
{
    std::vector<pkm> ret;
    for (Json::Value it : Ja) {
        ret.push_back(J2pkm(it));
    }
    return ret;
}
std::vector<text_menu *> Ja2text_menu(const Json::Value &Ja)
{
    std::vector<text_menu *> ret;
    for (auto it : Ja) {
        if (it.isObject()) {
            ret.push_back(J2text_menu(it));
        }
        else if (it.isString()) {
            ; // ignore, process at add_Ja_options
        }
        else {
            fmt::print("Process {} error, invalid option type.", Ja.toStyledString());
        }
    }
    return ret;
}
void add_Ja_options(const Json::Value &Ja, text_menu *f)
{
    for (auto it : Ja) {
        if (it.isString()) {
            menu_options.emplace_back(f, it.asString());
        }
    }
}

std::function<void(player &)> action_function_finder(const Json::Value J)
{
    if (!J.isNull()) {
        if (J.isString()) {
            std::string act_type = J.asString();
            if (!act_type.empty()) {
                try {
                    return map_finder(act_type, action_mapper);
                }
                catch (...) {
                    return [act_type = const_cast<const std::string &>(act_type)](player &p) {
                        p.output2user(get_action_text(act_type, p));
                    };
                }
            }
            else {
                return nullptr;
            }
        }
        else {
            fmt::print("action must be a string");
            return nullptr;
        }
    }
    else {
        return nullptr;
    }
}

text_menu *J2text_menu(const Json::Value &J)
{
    text_menu *p = new text_menu(J["title"].asString(), J["choose_text"].asString(), Ja2text_menu(J["options"]));
    add_Ja_options(J["options"], p);
    if (J.isMember("uid"))
        p->set_uid(J["uid"].asString());

    if (J.isMember("action") && !J["action"].isNull()) {
        p->action = action_function_finder(J["action"]);
    }
    if (J.isMember("is_choose")) {
        p->is_choose = J["is_choose"].asBool();
    }
    if (p->is_choose) {
        try {
            if (!J.isMember("choose_cb") || !J.isMember("get_choose_set")) {
                fmt::print("choose menu syntax error\n");
            }
            else {
                p->choose_cb = map_finder(J["choose_cb"].asString(), choose_callback_mapper);
                p->get_choose_set = map_finder(J["get_choose_set"].asString(), get_choose_set_mapper);
            }
        }
        catch (...) {
            fmt::print("choose menu syntax error\n");
        }
    }
    if (J.isMember("need_back")) {
        p->need_back = J["need_back"].asBool();
    }
    if (J.isMember("father") && !J["father"].asString().empty()) {
        menu_fathers.emplace_back(p, J["father"].asString());
    }
    // TODO
    return p;
}

Json::Value base62J(const base6 &b)
{
    Json::Value J(Json::arrayValue);
    J.append(b.hp);
    J.append(b.atk);
    J.append(b.stk);
    J.append(b.df);
    J.append(b.sdf);
    J.append(b.spd);
    return J;
}

Json::Value pkm2J(const pkm &p)
{
    Json::Value J;
    J["id"] = p.id;
    J["name"] = p.name;
    J["get_time"] = p.get_time;
    J["get_place"] = p.get_place;
    J["gender"] = static_cast<int>(p.gend);
    J["level"] = p.level;
    J["exp_curr"] = p.exp_curr;
    J["IV"] = base62J(p.IV);
    J["bp"] = base62J(p.base_points);
    J["friendship"] = p.friendship;
    J["battle_status"] = static_cast<int>(p.bstatus);
    Json::Value skill(Json::arrayValue);
    for (int i = 0; i < 4; ++i) {
        skill.append(p.skills[i]);
    }
    J["skill"] = skill;
    Json::Value upp(Json::arrayValue);
    for (int i = 0; i < 4; ++i) {
        upp.append(p.used_pp[i]);
    }
    J["used_pp"] = upp;
    J["nature"] = p.nature;
    J["hpreduced"] = p.hpreduced;
    J["is_shiny"] = p.is_shiny;
    J["carry_item"] = p.carried_item;
    return J;
}

Json::Value pkm2Ja(const std::vector<pkm> &vp)
{
    Json::Value Ja(Json::arrayValue);
    for (const auto &p : vp) {
        Ja.append(pkm2J(p));
    }
    return Ja;
}

npc *J2npc(const Json::Value &J)
{
    return new npc(J["id"].asString(), J["name"].asString(), map_finder(J["text_menu"].asString(), text_menu_mapper),
                   J.isMember("action") ? action_function_finder(J["action"]) : nullptr);
}