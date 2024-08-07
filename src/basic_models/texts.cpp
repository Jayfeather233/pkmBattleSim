#include "texts.hpp"

#include <fmt/core.h>

std::string get_action_text(const std::string &typ, const player &p1)
{
    auto it = action_text.find(typ);
    if (it == action_text.end()) {
        return fmt::format("action_text for {} not found.", typ);
    }
    std::string ret = it->second[get_random(action_text[typ].size())];
    my_string_format(ret, p1);
    return ret;
}

std::string get_levelup_text(const pkm &p)
{
    return fmt::format("cong, {} up to {}", p.get_name(), (int)p.level); // TODO
}

std::string base6str[6] = {
    "HP", "攻击", "防御", "特攻", "特防", "速度",
};

std::string nature2str[25] = {
    "勤奋", "怕寂寞", "固执",   "顽皮", "勇敢", "大胆", "坦率", "淘气", "乐天",
    "悠闲", "内敛",   "慢吞吞", "害羞", "马虎", "冷静", "温和", "温顺", "慎重",
    "浮躁", "自大",   "胆小",   "急躁", "爽朗", "天真", "认真"};