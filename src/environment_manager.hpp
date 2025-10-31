#pragma once
#include "building.hpp"
#include "traffic_map.hpp"
#include <SFML/Graphics.hpp>
#include <vector>

class EnvironmentManager
{
public:
    void initialize_from_traffic_map(TrafficMap& map);

    void enable_day_night_cycle(bool enabled) { m_day_night = enabled; }
    void update(sf::Time dt);
    void draw_environment(sf::RenderWindow& window) const;

    sf::Color get_ambient_color() const;

private:
    std::vector<Building> m_buildings;

    // day/night cycle
    bool m_day_night = false;
    float m_time = 0.f; // seconds

    // helpers
    void place_decorations_along_road(const sf::Vector2f& start,
                                      const sf::Vector2f& end,
                                      float road_width);
    void place_buildings_near_point(const sf::Vector2f& p, float spacing);
};
