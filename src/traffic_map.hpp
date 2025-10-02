#pragma once

#include "road.hpp"
#include "junction.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <unordered_map>
#include <memory>

/**
 * This class manages the entire traffic network.
 * It owns (creates, destroys, updates, renders) all map objects (currently Road and Junction).
 */

class TrafficMap
{
public:
    TrafficMap() = default;

    // Adds a new road starting and ending at junctions at specified coordinates.
    // Junctions are - retrieved if they already exist
    //               - created if they do not
    void add_road(const sf::Vector2f& start_pos, const sf::Vector2f& end_pos);

    // Updates the state of all map elements with respect to time elapsed
    void update(sf::Time elapsed);

    // Draws all map objects on a render window
    void draw(sf::RenderWindow& window) const;

    // Retrieves road by its index in the storage vector
    std::weak_ptr<Road> get_road(size_t index) const;

    // Clears all roads and junctions from the map
    void clear();

private:
    // Returns an existing junction at a given position or creates a new one
    std::shared_ptr<Junction> get_or_create_junction(const sf::Vector2f& position);

    std::vector<std::shared_ptr<Road>> m_roads;
    std::unordered_map<sf::Vector2f, std::shared_ptr<Junction>, Junction_Hash> m_junctions;
};
