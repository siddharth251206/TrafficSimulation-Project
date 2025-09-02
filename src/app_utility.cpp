#include "app_utility.hpp"
#include "road.hpp"
#include <random>

RNG& RNG::instance()
{
    static RNG object;
    return object;
}

Junction_Table& Junction_Table::instance()
{
    static Junction_Table object;
    return object;
}

RNG::RNG() : gen(std::random_device{}()) {}

size_t RNG::getIndex(size_t lo, size_t hi)
{
    std::uniform_int_distribution<size_t> dis(lo, hi);
    return dis(gen);
}

size_t Junction_Hash::operator()(const sf::Vector2f& j) const noexcept
{
    return std::hash<float>{}(j.x) ^ (std::hash<float>{}(j.y) << 1);
}

Junction& Junction_Table::insert_junction(Junction& j)
{
    junction_list.insert({ j.get_location(), j }); // Working on this, I know it is wrong.
    return j;
}

const Junction* Junction_Table::search_junction(sf::Vector2f location) const
{
    auto it = junction_list.find(location);
    if (it != junction_list.end())
        return &(it->second);
    else
        return nullptr;
}

void Junction_Table::delete_junction(const Junction& j) { junction_list.erase(j.get_location()); }
