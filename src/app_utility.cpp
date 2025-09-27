#include "app_utility.hpp"
#include "junction.hpp"
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
    std::uniform_int_distribution<size_t> dis(lo, hi);
    return dis(gen);
}

// =================== Junction_Hash ===================

size_t Junction_Hash::operator()(const sf::Vector2f& j) const noexcept
{
    // Simple XOR hash combining x and y
    return std::hash<float>{}(j.x) ^ (std::hash<float>{}(j.y) << 1);
}

// =================== Junction_Table ===================

Junction_Table& Junction_Table::instance()
{
    static Junction_Table object;
    return object;
}

std::shared_ptr<Junction> Junction_Table::insert_junction(std::shared_ptr<Junction> j)
{
    junction_list[j->get_location()] = j;
    return j;
}

std::shared_ptr<Junction> Junction_Table::search_junction(sf::Vector2f location) const
{
    auto it = junction_list.find(location);
    if (it != junction_list.end())
        return it->second;
    return nullptr;
}

void Junction_Table::delete_junction(sf::Vector2f location)
{
    junction_list.erase(location);
}
