#include "app_utility.hpp"
#include <functional>

// =================== RNG ===================

RNG& RNG::instance()
{
    static RNG object;
    return object;
}

RNG::RNG() : gen(std::random_device{}()) {}

size_t RNG::getIndex(size_t lo, size_t hi)
{
    if (lo > hi)
        return lo;
    std::uniform_int_distribution<size_t> dis(lo, hi);
    return dis(gen);
}
