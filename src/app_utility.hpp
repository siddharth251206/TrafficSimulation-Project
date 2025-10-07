#pragma once
#include <SFML/System/Vector2.hpp>
#include <memory>
#include <random>

// Convert radians to degrees (helper for SFML angle rotations)
float to_degrees(float radians);

bool point_in_circle(sf::Vector2f centre, float radius, sf::Vector2f point);

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
    float getFloat(float lo, float hi);
};

// Hash functor for sf::Vector2i (used as key for junctions)
struct Junction_Hash
{
    size_t operator()(const sf::Vector2i& j) const noexcept;
};
