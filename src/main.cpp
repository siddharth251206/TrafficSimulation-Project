#include "traffic_map.hpp"
#include "traffic_light.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <iostream>
#include <filesystem>
#include <optional>
#include <string>

// Resolve an asset path like "assets/premium_car.png" by searching common
// working directories (., .., ../.., ../../.., ../../../..)
static std::optional<std::string> resolve_asset_path(const std::string& relative)
{
    namespace fs = std::filesystem;
    const fs::path rel(relative);
    const fs::path bases[] = {
        fs::path{"."},
        fs::path{".."},
        fs::path{"../.."},
        fs::path{"../../.."},
        fs::path{"../../../.."}
    };

    for (const auto& base : bases)
    {
        const fs::path candidate = base / rel;
        if (fs::exists(candidate))
            return candidate.string();
    }
    return std::nullopt;
}

// Try to load a texture from a relative asset path, resolving across common bases.
// Returns true on success and logs what happened.
static bool try_load_texture(sf::Texture& tex, const std::string& relPath, const char* label)
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

int main()
{
    constexpr unsigned int width = 1200;
    constexpr unsigned int height = 800;
    sf::RenderWindow window{ sf::VideoMode{ { width, height } }, "Traffic Simulator" };
    window.setFramerateLimit(60);

    TrafficMap traffic_map;

    // Create a four-way intersection
    const sf::Vector2f center = {600.f, 400.f};
    const sf::Vector2f north = {600.f, 100.f};
    const sf::Vector2f south = {600.f, 700.f};
    const sf::Vector2f east = {1100.f, 400.f};
    const sf::Vector2f west = {100.f, 400.f};

    traffic_map.add_double_road(north, center); // Road 0
    traffic_map.add_double_road(south, center); // Road 1
    traffic_map.add_double_road(west, center);  // Road 2
    traffic_map.add_double_road(east, center);  // Road 3

    // Install traffic lights
    if (auto intersection = traffic_map.get_junction(center)) 
        intersection->install_light(sf::seconds(12)); // North -> Center

    // --- Load Car Texture (for sprite-based cars) ---
    sf::Texture car_texture;
    bool loaded = false;
    loaded = try_load_texture(car_texture, "assets/premium_car.png", "premium car");
    if (!loaded) loaded = try_load_texture(car_texture, "assets/advanced_car.png", "advanced car");
    if (!loaded) loaded = try_load_texture(car_texture, "assets/car.png", "simple car");

    // Car Spawner Logic
    sf::Clock spawn_timer;
    int max_cars = 20;
    int spawned_count = 0;

    sf::Clock clock;
    while (window.isOpen())
    {
        sf::Time elapsed = clock.restart();
        while (const std::optional<sf::Event> event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        // Spawn cars on the north and west roads
        if (spawned_count < max_cars && spawn_timer.getElapsedTime().asSeconds() > 1.5f) {
            if (auto road = traffic_map.get_double_road(0)) { // North road
                // Pass texture pointer if loaded, otherwise nullptr and car will fallback to rectangle
                road->add_to_forward(std::make_unique<Car>(road->get_forward(), loaded ? &car_texture : nullptr));
                spawned_count++;
            }
            if (auto road = traffic_map.get_double_road(2)) { // West road
                road->add_to_forward(std::make_unique<Car>(road->get_forward(), loaded ? &car_texture : nullptr));
                spawned_count++;
            }
            spawn_timer.restart();
        }

        traffic_map.update(elapsed);

        window.clear(sf::Color(20, 20, 40));
        traffic_map.draw(window);
        window.display();
    }
}
