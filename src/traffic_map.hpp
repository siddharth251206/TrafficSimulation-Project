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

    void add_road(const sf::Vector2f& start_pos, const sf::Vector2f& end_pos);

    void add_double_road(
        const sf::Vector2f& start_pos,
        const sf::Vector2f& end_pos,
        float width = 25.f,
        bool need_divider = true
    );

    void update(sf::Time elapsed);
    void draw(sf::RenderWindow& window) const;

    std::shared_ptr<Road> get_single_road(size_t index) const;
    std::shared_ptr<DoubleRoad> get_double_road(size_t index) const;
    std::shared_ptr<Junction> get_junction(const sf::Vector2f& position);

    void clear();

private:
    std::shared_ptr<Junction> get_or_create_junction(const sf::Vector2f& position);
    std::vector<std::shared_ptr<DoubleRoad>> m_double_roads;
    std::vector<std::shared_ptr<Road>> m_single_roads;
    std::unordered_map<sf::Vector2i, std::vector<std::shared_ptr<Junction>>, Junction_Hash>
        m_junctions;
};
