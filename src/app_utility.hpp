#pragma once
#include <SFML/System/Vector2.hpp>
#include <random>
#include <unordered_map>
#include <memory>

class Junction;

// RNG singleton
class RNG
{
private:
    RNG();
    ~RNG() = default;

    RNG(const RNG&) = delete;
    RNG(RNG&&) = delete;
    RNG& operator=(const RNG&) = delete;
    RNG& operator=(RNG&&) = delete;

    std::mt19937 gen;

public:
    static RNG& instance();
    size_t getIndex(size_t lo, size_t hi);
};

// Hash functor for sf::Vector2f (used as key for junctions)
struct Junction_Hash
{
    size_t operator()(const sf::Vector2f& j) const noexcept;
};

// Singleton table to store all junctions
class Junction_Table
{
private:
    Junction_Table() = default;
    ~Junction_Table() = default;

    Junction_Table(const Junction_Table&) = delete;
    Junction_Table(Junction_Table&&) = delete;
    Junction_Table& operator=(const Junction_Table&) = delete;
    Junction_Table& operator=(Junction_Table&&) = delete;

    std::unordered_map<sf::Vector2f, std::shared_ptr<Junction>, Junction_Hash> junction_list;

public:
    static Junction_Table& instance();

    std::shared_ptr<Junction> insert_junction(std::shared_ptr<Junction> j);
    std::shared_ptr<Junction> search_junction(sf::Vector2f location) const;
    void delete_junction(sf::Vector2f location);
};

