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

pkm_base J2pkm_base(const Json::Value &J)
{
    return pkm_base(J2base6(J["sp"]),
                    {static_cast<element_types>(J["tp"][0].asInt()),
                     static_cast<element_types>(J["tp"][1].asInt())},
                    J["id"].asInt());
}

pkm J2pkm(const Json::Value &J)
{
    return pkm(pkm_base(J2pkm_base(J)), J["name"].asString(),
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

Json::Value pkm_base2J(const pkm_base &p)
{
    Json::Value J;
    J["sp"] = base62J(p.species_points);
    Json::Value tp(Json::arrayValue);
    tp.append(static_cast<int>(p.typ[0]));
    tp.append(static_cast<int>(p.typ[1]));
    J["tp"] = tp;
    J["id"] = p.id;
    return J;
}

Json::Value pkm2J(const pkm &p)
{
    Json::Value J = pkm_base2J(p);
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