#include "utils.hpp"

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