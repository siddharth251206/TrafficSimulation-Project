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

// =================== Junction_Hash ===================

size_t Junction_Hash::operator()(const sf::Vector2f& j) const noexcept
{
    // A common way to combine hashes for a 2D vector
    size_t h1 = std::hash<float>{}(j.x);
    size_t h2 = std::hash<float>{}(j.y);
    // A good hash combination formula to reduce collisions
    return h1 ^ (h2 << 1);
}
