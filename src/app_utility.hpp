#pragma once

#include "road.hpp"
#include <random>
#include <unordered_set>

template<typename T>
class Singleton
{
private:
    Singleton(const Singleton&) = delete;
    Singleton(Singleton&&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    Singleton& operator=(Singleton&&) = delete;

public:
    static T& instance();
};

class RNG : public Singleton<RNG>
{
    friend class Singleton<RNG>;

private:
    RNG();
    std::mt19937 gen; // Huge object, don't create many instances.

public:
    size_t getIndex(size_t lo, size_t hi);
};

struct Junction_Hash
{
    size_t operator()(const Junction& j) const
    {
        return (
            std::hash<float>{}(j.get_location().x) ^ ((std::hash<float>{}(j.get_location().y)) << 1)
        );
    }
};

class Junction_Table : public Singleton<Junction_Table>
{
    friend class Singleton<Junction_Table>;

private:
    Junction_Table();
    // Hash table to store list of all junctions.
    std::unordered_set<sf::Vector2f, Junction_Hash> junction_list;

public:
    void insert_junction(const Junction &j);
    bool search_junction(sf::Vector2f location);
};