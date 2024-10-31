#include "qqapi.hpp"

extern "C" processable *create_t()
{
    init_predefs();
    return new pkmbattle();
}
extern "C" void destroy_t(processable *p)
{
    remove_predefs();
    delete p;
}