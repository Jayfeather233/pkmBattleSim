#include "myutils.hpp"

void replaceAll(std::string &str, const std::string &from,
                const std::string &to)
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