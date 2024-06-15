#include "threads.hpp"
#include "skills.hpp"

int main(){
    init_predefs();
    singleplayerthread x = singleplayerthread(233);
    x.run();
}