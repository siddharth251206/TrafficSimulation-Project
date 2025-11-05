#pragma once

#include "app_utility.hpp"
#include "double_road.hpp"
#include "junction.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <string> // For filename
#include <unordered_map>
#include <vector>

class TrafficMap
{
public:
    TrafficMap() = default;

    // --- NEW ---
    // Public-facing map loader
    void load_map_from_file(
        const std::string& filename,
        sf::Time green_duration,
        sf::Time yellow_duration
    );
    // -----------

    void add_road(const sf::Vector2f& start_pos, const sf::Vector2f& end_pos);

    void add_double_road(
        const sf::Vector2f& start_pos,
        const sf::Vector2f& end_pos,
        float width = 25.f,
        bool need_divider = true
    );

    // --- NEW FUNCTION ---
    // Finds a junction at a specific position and tells it to install a light.
    void install_light_at_junction(
        const sf::Vector2f& position,
        sf::Time green_duration,
        sf::Time yellow_duration
    );
    // --------------------

    // --- REMOVED ---
    // void install_all_lights(...);
    // -----------------

    void update(sf::Time elapsed);
    void draw(sf::RenderWindow& window) const;

    std::shared_ptr<Road> get_single_road(size_t index) const;
    std::shared_ptr<DoubleRoad> get_double_road(size_t index) const;
    std::shared_ptr<Junction> get_junction(const sf::Vector2f& position);
    [[nodiscard]] std::shared_ptr<Road> get_random_road();
    [[nodiscard]] size_t get_car_count() const;

    void clear();

private:
    std::shared_ptr<Junction> get_or_create_junction(const sf::Vector2f& position);
    std::vector<std::shared_ptr<DoubleRoad>> m_double_roads;
    std::vector<std::shared_ptr<Road>> m_single_roads;
    std::vector<std::shared_ptr<Road>> m_all_roads;
    std::unordered_map<sf::Vector2i, std::vector<std::shared_ptr<Junction>>, Junction_Hash>
        m_junctions;
};