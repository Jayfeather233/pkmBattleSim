#pragma once

#include <cctype>
#include <random>

using u32 = uint_least32_t;
using engine = std::mt19937;

static std::random_device os_seed;
static const u32 seed = os_seed();
static engine generator = engine(seed);

inline u32 get_random(u32 maxi)
{
    std::uniform_int_distribution<u32> uni_dis =
        std::uniform_int_distribution<u32>(0, maxi - 1);
    return uni_dis(generator);
}
