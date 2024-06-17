#include "utils.hpp"
#include "pkm.hpp"

std::vector<int> Ja2Vec(const Json::Value &Ja)
{
    std::vector<int> ret;
    for (Json::Value it : Ja) {
        ret.push_back(it.asInt());
    }
    return ret;
}

std::vector<std::pair<int,int>> Ja2VecP(const Json::Value &Ja)
{
    std::vector<std::pair<int,int>> ret;
    for (Json::Value it : Ja) {
        ret.push_back(std::make_pair(it["level"].asInt(), it["skill_id"].asInt()));
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
        J[0].asInt(), J[1].asInt(), J[2].asInt(),
        J[3].asInt(), J[4].asInt(), J[5].asInt(),
    };
}

element_types J2et(const Json::Value &J){
    try{
        return static_cast<element_types>(J.asInt());
    } catch (...){
        return str2et(J.asString());
    }
}

pkm_base J2pkm_base(const Json::Value &J) {
    // Extract species_points
    base6 sp = J2base6(J["sp"]);

    // Extract element_types
    std::array<element_types, 2> et = {
        J2et(J["tp"][0]),
        J2et(J["tp"][1])
    };

    // Extract poss_abilities
    std::vector<std::pair<int, float>> abilities;
    for (const auto& ability : J["poss_abilities"]) {
        abilities.push_back(std::make_pair(ability["id"].asInt(), ability["poss"].asFloat()));
    }

    // Extract aquire_base_point
    base6 abp = J2base6(J["aquire_base_point"]);

    // Create and return the pkm_base object
    return pkm_base(
        J["name"].asString(),
        sp,
        et,
        J["id"].asInt(),
        J["category"].asString(),
        abilities,
        J["height"].asFloat(),
        J["weight"].asFloat(),
        static_cast<u_char>(J["catch_rate"].asUInt()),
        J["gender_ratio"].asInt(),
        abp,
        static_cast<u_char>(J["exp_acc_speed"].asUInt()),
        J["base_exp"].asInt()
    );
}

pkm J2pkm(const Json::Value &J)
{
    return pkm(pkm_list[J["id"].asInt()], J["name"].asString(),
               static_cast<gender>(J["gender"].asInt()), J["level"].asInt(),
               J["exp_need"].asInt(), J["exp_curr"].asInt(), J2base6(J["IV"]),
               J2base6(J["bp"]), J["friendship"].asInt(),
               static_cast<battle_status>(J["battle_status"].asInt()),
               {J["skill"][0].asInt(), J["skill"][1].asInt(),
                J["skill"][2].asInt(), J["skill"][3].asInt()},
               J["nature"].asInt(), J["hpreduced"].asInt(),
               J["is_shiny"].asBool());
}
std::vector<pkm> Ja2pkm(const Json::Value &Ja)
{
    std::vector<pkm> ret;
    for (Json::Value it : Ja) {
        ret.push_back(J2pkm(it));
    }
    return ret;
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
    J["name"] = p.name;
    J["gender"] = static_cast<int>(p.gend);
    J["level"] = p.level;
    J["exp_need"] = p.exp_need;
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
    J["nature"] = p.nature;
    J["hpreduced"] = p.hpreduced;
    J["is_shiny"] = p.is_shiny;
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