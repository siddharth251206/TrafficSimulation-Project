#pragma once

#include "road.hpp"
#include <random>
#include <unordered_set>

class RNG
{
private:
    RNG() : gen(std::random_device{}()) {}// private constructor
    ~RNG() = default;

    // Prevent copying/moving
    RNG(const RNG&) = delete;
    RNG(RNG&&) = delete;
    RNG& operator=(const RNG&) = delete;
    RNG& operator=(RNG&&) = delete;

    std::mt19937 gen;// Huge object, only one instance

public:
    static RNG& instance();
    size_t getIndex(size_t lo, size_t hi);
};

// Hash functor for Junctions
struct Junction_Hash
{
    size_t operator()(const sf::Vector2f& j) const noexcept;
};


class Junction_Table
{
private:
    Junction_Table() = default;
    ~Junction_Table() = default;

    // Prevent copying/moving
    Junction_Table(const Junction_Table&) = delete;
    Junction_Table(Junction_Table&&) = delete;
    Junction_Table& operator=(const Junction_Table&) = delete;
    Junction_Table& operator=(Junction_Table&&) = delete;

    std::unordered_set<sf::Vector2f, Junction_Hash> junction_list;

public:
    static Junction_Table& instance();
    void insert_junction(const Junction& j);
    bool search_junction(sf::Vector2f location) const;
};
