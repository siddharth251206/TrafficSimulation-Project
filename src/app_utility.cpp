#include "app_utility.hpp"
#include <random>

template<typename T>
T& Singleton<T>::instance()
{
    static T object;
    return object;
}

RNG::RNG() : gen(std::random_device{}()) {}

size_t RNG::getIndex(size_t lo, size_t hi)
{
    std::uniform_int_distribution<size_t> dis(lo, hi);
    return dis(gen);
}

void Junction_Table::insert_junction(const Junction &j){
    Junction_Table::instance().junction_list.insert(j.get_location());
}

bool Junction_Table::search_junction(sf::Vector2f location){
    auto it = Junction_Table::instance().junction_list.find(location);
    return true; // I will deal with this next, stop pointing out things :/
}
