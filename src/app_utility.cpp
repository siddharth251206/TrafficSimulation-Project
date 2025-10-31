#include "app_utility.hpp"
#include <iostream>
#include <filesystem>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <functional>
#include <random>
#include <optional>

bool point_in_circle(sf::Vector2f centre, float radius, sf::Vector2f point)
{
    return ((centre.y - point.y) * (centre.y - point.y))
               + ((centre.x - point.x) * (centre.x - point.x))
           <= (radius * radius);
}

// =================== RNG ===================

RNG& RNG::instance()
{
    static RNG object;
    return object;
}

RNG::RNG() : gen(std::random_device{}()) {}

size_t RNG::getIndex(size_t lo, size_t hi)
{
    if (lo > hi)
        return lo;
    std::uniform_int_distribution<size_t> dis(lo, hi);
    return dis(gen);
}

float RNG::getFloat(float lo, float hi)
{
    if (lo > hi)
        return lo;
    std::uniform_real_distribution<float> dis(lo, hi);
    return dis(gen);
}

// =================== Junction_Hash ===================

size_t Junction_Hash::operator()(const sf::Vector2i& j) const noexcept
{
    // A common way to combine hashes for a 2D vector
    size_t h1 = std::hash<int>{}(j.x);
    size_t h2 = std::hash<int>{}(j.y);
    // A good hash combination formula to reduce collisions
    return h1 ^ (h2 << 1);
}

// =================== AssetHelper ===================

std::optional<std::string> AssetHelper::resolve_asset_path(const std::string& relative)
{
    namespace fs = std::filesystem;
    const fs::path rel(relative);
    const fs::path bases[] = { fs::path{ "." },
                               fs::path{ ".." },
                               fs::path{ "../.." },
                               fs::path{ "../../.." },
                               fs::path{ "../../../.." } };

    for (const auto& base : bases)
    {
        const fs::path candidate = base / rel;
        if (fs::exists(candidate))
            return candidate.string();
    }
    return std::nullopt;
}

bool AssetHelper::try_load_texture(sf::Texture& tex, const std::string& relPath, const char* label)
{
    if (auto full = resolve_asset_path(relPath))
    {
        if (tex.loadFromFile(*full))
        {
            std::cout << "Loaded " << label << " from: " << *full << "\n";
            return true;
        }
        else
        {
            std::cerr << "Found file but failed to load: " << *full << "\n";
        }
    }
    return false;
}

bool AssetHelper::try_load_font(sf::Font& font)
{
    namespace fs = std::filesystem;
    // Candidate relative asset paths
    const char* rels[] = {
        "assets/fonts/Roboto-Regular.ttf",
        "assets/fonts/DejaVuSans.ttf",
        "assets/DejaVuSans.ttf",
        "assets/Arial.ttf"
    };
    for (const char* r : rels)
    {
        if (auto full = resolve_asset_path(r))
        {
            if (font.openFromFile(*full))
            {
                std::cout << "Loaded font from: " << *full << "\n";
                return true;
            }
        }
    }

    // OS-specific common locations
    const char* absolutes[] = {
#ifdef _WIN32
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/segoeui.ttf",
#endif
#ifdef __APPLE__
        "/System/Library/Fonts/SFNS.ttf",
        "/System/Library/Fonts/Supplemental/Arial.ttf",
        "/Library/Fonts/Arial.ttf",
#endif
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/freefont/FreeSans.ttf"
    };
    for (const char* abs : absolutes)
    {
        fs::path p(abs);
        if (fs::exists(p))
        {
            if (font.openFromFile(p.string()))
            {
                std::cout << "Loaded system font from: " << p.string() << "\n";
                return true;
            }
        }
    }
    std::cerr << "Failed to load any fallback font. UI text will be disabled.\n";
    return false;
}
