#pragma once

#include "basic.hpp"

#include <cctype>
#include <random>
#include <utility>
#include <vector>

using u32 = uint_least32_t;
using engine = std::mt19937;

static std::random_device os_seed;
static const u32 seed = os_seed();
static engine generator = engine(seed);

// [0, maxi)
inline u32 get_random(u32 maxi)
{
    std::uniform_int_distribution<u32> uni_dis =
        std::uniform_int_distribution<u32>(0, maxi - 1);
    return uni_dis(generator);
}
inline int get_random(int maxi)
{
    std::uniform_int_distribution<int> uni_dis =
        std::uniform_int_distribution<int>(0, maxi - 1);
    return uni_dis(generator);
}
inline size_t get_random(size_t maxi)
{
    std::uniform_int_distribution<size_t> uni_dis =
        std::uniform_int_distribution<size_t>(0, maxi - 1);
    return uni_dis(generator);
}

inline base6 random_base6(int maxi)
{
    return (base6){get_random(maxi), get_random(maxi), get_random(maxi),
                   get_random(maxi), get_random(maxi), get_random(maxi)};
}

template <typename T>
T get_possi_random(const std::vector<std::pair<T, float>> &vec)
{
    std::uniform_real_distribution<float> uni_dis =
        std::uniform_real_distribution<float>(0, 1);
    float prob = uni_dis(generator);
    for (auto it : vec) {
        if (prob <= it.second) {
            return it.first;
        }
        else {
            prob -= it.second;
        }
    }
    return T();
}

template <typename T>
size_t get_possi_random_(const std::vector<std::pair<T, float>> &vec)
{
    std::uniform_real_distribution<float> uni_dis =
        std::uniform_real_distribution<float>(0, 1);
    float prob = uni_dis(generator);
    for (size_t i = 0; i < vec.size(); ++i) {
        if (prob <= vec[i].second) {
            return i;
        }
        else {
            prob -= vec[i].second;
        }
    }
    return 0;
}