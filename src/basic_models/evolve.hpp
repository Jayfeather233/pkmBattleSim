#pragma once

#include "pkm.hpp"

enum evolve_trigger{
    level_up,
    user_item,
    trade
};

bool check_evolve(const pkm &p, evolve_trigger et, int arg){

}

pkm create_evolve(const pkm &p, evolve_trigger et, int arg){

}

void init_evolve_chains(){
    // TODO
}