#ifndef ITEM_H
#define ITEM_H

#include <functional>
#include <vector>
#include "player.hpp"

class basic_item {
public:
    uint32_t item_id;
    uint32_t value; // sell

    // the bool only imply whether item amount will be decrease.
    // do not use it as a successful apply
    // message should be output here
    std::function<bool(player *p)> direct_use;
    std::function<bool(player *pl, pkm *p)> pkm_carry;
    std::function<bool(player *pl, pkm *p)> apply_pkm;

    bool can_be_used_in_battle;
    bool can_be_used_normal;
    bool is_important_item; // like Exp. Share
    // all not important items can be carried by pkm

    std::function<bool(pkm *p)> battle_auto_use_trigger; // TODO: maybe add a battle status to indicate when the trigger is called

    basic_item(uint32_t id, uint32_t v) : item_id(id), value(v) {}
};

// basic_item_list[id] == item
// in player, store list<id, amount> item_list
extern std::vector<basic_item> basic_item_list;

class pokeball : public basic_item{
public:
    enum ball_type {
        POKE_BALL,
        GREAT_BALL,
        ULTRA_BALL,
    };

    


    pokeball(uint32_t id, uint32_t v, ball_type bt) : basic_item(id, v) {

    }
};


#endif // ITEM_H