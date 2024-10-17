#include "myutils.hpp"

#include <chrono>
#include <ctime>
#include <fmt/core.h>
#include <fmt/ranges.h>

void replaceAll(std::string &str, const std::string &from, const std::string &to)
{
    size_t startPos = 0;
    while ((startPos = str.find(from, startPos)) != std::string::npos) {
        str.replace(startPos, from.length(), to);
        startPos += to.length(); // In case 'to' contains 'from', like replacing
                                 // 'x' with 'yx'
    }
}

int get_next_int(player &p, std::string text)
{
    if (text.length()) {
        text = "\n" + text;
    }
    while (1) {
        try {
            return std::stoi(p.get_user_input());
        }
        catch (...) {
            p.output2user("invalid input." + text);
        }
    }
}

std::string gender2string(const gender &g)
{
    if (g == gender::FEMALE) {
        return "♀";
    }
    else if (g == gender::MALE) {
        return "♂";
    }
    else {
        return "";
    }
}

std::string eletype2string(const element_types &a)
{
    if (a == element_types::BUG) {
        return "虫";
    }
    else if (a == element_types::GRASS) {
        return "草";
    }
    else if (a == element_types::POISON) {
        return "毒";
    }
    else {
        // TODO: complete here
    }
}

std::string eletype2string(const std::array<element_types, 2> a)
{
    if (a[1] != element_types::EMPTY) {
        return fmt::format("{}, {}", eletype2string(a[0]), eletype2string(a[1]));
    }
    else {
        return eletype2string(a[0]);
    }
}

void my_string_format(std::string &s, const player &pl, const pkm *pk)
{
    replaceAll(s, "$u", pl.name);
    if (pl.mt.menu_choose_pokemon != -1) {
        replaceAll(s, "$1", pl.get_choose_pkm_const()->get_name());
    }
    if (pk != nullptr) {
        replaceAll(s, "$e", pk->get_name());
    }
    replaceAll(s, "$p", fmt::format("{}({}/{})", pl.pls->name, pl.mt.move_point, pl.pls->meet_points));
}

std::string nowtime_string()
{
    std::time_t nt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    tm tt = *localtime(&nt);
    return fmt::format("{}年{}月{}日", tt.tm_year, tt.tm_mon + 1, tt.tm_mday);
}