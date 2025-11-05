#pragma once
#include "building.hpp"
#include "traffic_map.hpp"
#include "urban_element.hpp"
#include <SFML/Graphics.hpp>
#include <vector>

struct Zone
{
    sf::FloatRect bounds;
    ZoneType type;
    float density; // Building density 0.0 - 1.0
};

class EnvironmentManager
{
public:
    void initialize_from_traffic_map(TrafficMap& map);
    void draw_environment(sf::RenderWindow& window) const;

private:
    std::vector<Building> m_buildings;
    std::vector<UrbanElement> m_urban_elements;
    std::vector<Zone> m_zones;

    // Initialization helpers
    void define_zones();
    void populate_residential_zone(const Zone& zone);
    void populate_commercial_zone(const Zone& zone);
    void populate_public_zone(const Zone& zone);
    void add_street_infrastructure();
    void add_parks_and_greenery();

    // Placement helpers
    void place_building(
        const sf::Vector2f& position,
        const sf::Vector2f& size,
        BuildingType type,
        float rotation = 0.f
    );
    void place_urban_element(const sf::Vector2f& position, UrbanElementType type);
    bool is_position_clear(const sf::Vector2f& position, float radius) const;
    float get_road_angle(const sf::Vector2f& position) const;

    // Reference to roads (for positioning)
    std::vector<std::pair<sf::Vector2f, sf::Vector2f>> m_road_segments;
};
