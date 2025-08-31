#include "app_utility.hpp"
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

void Junction_Table::insert_junction(const Junction& j) { junction_list.insert(j.get_location()); }

bool Junction_Table::search_junction(sf::Vector2f location) const
{
    return junction_list.find(location) != junction_list.end();
}
