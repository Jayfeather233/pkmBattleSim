#pragma once

#include "menu.hpp"

class npc {
public:
    std::string id;
    std::string name;
    const text_menu *npc_menu;
    std::function<void(player &p)> action;

    npc(const std::string &idx, const std::string &name, const text_menu *menux,
        const std::function<void(player &p)> &f)
        : id(idx), name(name), npc_menu(menux), action(f)
    {
    }
};

extern std::map<std::string, npc *> npc_mapper;

void npc_remove();