#pragma once
#include <SFML/System/Vector2.hpp>
#include <functional>
#include <random>

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
