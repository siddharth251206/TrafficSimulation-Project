#pragma once

#include "app_utility.hpp"
#include "double_road.hpp"
#include "junction.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <unordered_map>
#include <vector>
#include <string>

class TrafficMap
{
public:
    TrafficMap() = default;

    // UPDATE: Added max_speed parameter (defaults to 60 if not provided)
    void add_road(const sf::Vector2f& start_pos, const sf::Vector2f& end_pos, float max_speed = 60.f);

    // UPDATE: Added max_speed parameter
    void add_double_road(
        const sf::Vector2f& start_pos,
        const sf::Vector2f& end_pos,
        float max_speed = 60.f,
        float width = 25.f,
        bool need_divider = true
    );

    void update(sf::Time elapsed);
    void draw(sf::RenderWindow& window) const;

    std::shared_ptr<Road> get_single_road(size_t index) const;
    std::shared_ptr<DoubleRoad> get_double_road(size_t index) const;
    
    // Helper to find specific junctions for installing lights
    std::shared_ptr<Junction> get_junction(const sf::Vector2f& position);
    
    [[nodiscard]] std::shared_ptr<Road> get_random_road();
    [[nodiscard]] size_t get_car_count() const;

    void clear();

private:
    std::shared_ptr<Junction> get_or_create_junction(const sf::Vector2f& position);

    std::vector<std::shared_ptr<DoubleRoad>> m_double_roads;
    std::vector<std::shared_ptr<Road>> m_single_roads;
    std::vector<std::shared_ptr<Road>> m_all_roads;
    
    // Spatial Hash Map
    std::unordered_map<sf::Vector2i, std::vector<std::shared_ptr<Junction>>, Junction_Hash>
        m_junctions;

    // Constants for Grid Logic
    static constexpr float GRID_SIZE = 50.f;     // Increased slightly for better detection
    static constexpr float SNAP_RADIUS = 20.f;   // Distance to snap to existing junction
};