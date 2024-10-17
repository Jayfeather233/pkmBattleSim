#include "npc.hpp"

std::map<std::string, npc*> npc_mapper;

void npc_remove(){
    for(auto u : npc_mapper){
        delete u.second;
    }
}