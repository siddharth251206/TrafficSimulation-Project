#include "app_utility.hpp"
#include <SFML/Graphics.hpp>
#include <cmath>
#include <functional>
#include <random>

bool point_in_circle(sf::Vector2f centre, float radius, sf::Vector2f point)
{
    return ((centre.y - point.y) * (centre.y - point.y))
               + ((centre.x - point.x) * (centre.x - point.x))
           <= (radius * radius);
}

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

float RNG::getFloat(float lo, float hi)
{
    if (lo > hi)
        return lo;
    std::uniform_real_distribution<float> dis(lo, hi);
    return dis(gen);
}

// =================== Junction_Hash ===================

size_t Junction_Hash::operator()(const sf::Vector2i& j) const noexcept
{
    // A common way to combine hashes for a 2D vector
    size_t h1 = std::hash<int>{}(j.x);
    size_t h2 = std::hash<int>{}(j.y);
    // A good hash combination formula to reduce collisions
    return h1 ^ (h2 << 1);
}
