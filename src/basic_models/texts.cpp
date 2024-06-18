#include "texts.hpp"

#include <fmt/core.h>

std::string get_action_text(const std::string &typ, const pkm &p){
    auto it = action_text.find(typ);
    if(it == action_text.end()){
        return fmt::format("action_text for {} not found.", typ);
    }
    std::string ret = it->second[get_random(action_text[typ].size())];
    replaceAll(ret, "$1", p.get_name());
    return ret;
}