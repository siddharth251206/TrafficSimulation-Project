#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <memory>
#include <optional>
#include <random>

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

// Currently to handle textures.
class AssetHelper
{
    // Resolve an asset path like "assets/premium_car.png" by searching common
    // working directories (., .., ../.., ../../.., ../../../..)
    static std::optional<std::string> resolve_asset_path(const std::string& relative);

public:
    // Try to load a texture from a relative asset path, resolving across common bases.
    // Returns true on success and logs what happened.
    static bool try_load_texture(sf::Texture& tex, const std::string& relPath, const char* label);
};
