#include "texts.hpp"

#include <fmt/core.h>

std::string get_action_text(const std::string &typ, const player &p1){
    const pkm *p = p1.get_choose_pkm_const();
    auto it = action_text.find(typ);
    if(it == action_text.end()){
        return fmt::format("action_text for {} not found.", typ);
    }
    std::string ret = it->second[get_random(action_text[typ].size())];
    replaceAll(ret, "$u", p1.name);
    replaceAll(ret, "$1", p->get_name());
    return ret;
}