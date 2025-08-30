#pragma once

#include "car.hpp"

#include <SFML/Graphics.hpp>
#include <vector>

class Car;

class Road
{
public:
    // Construct a road based on given start and end points.
    Road(const sf::Vector2f& start, const sf::Vector2f& end);
    // Returns the unit vector direction of the road.
    [[nodiscard]] const sf::Vector2f& get_direction() const noexcept;
    // Gets the coordinates of the point in road at 'position' distance from the start.
    [[nodiscard]] sf::Vector2f get_point_at_distance(float position) const noexcept;
    // Updates the position of all cars in the road.
    void update(sf::Time elapsed);
    // Draws the road shape onto the world.
    void draw(sf::RenderWindow& window) const;
    // Add a car to the road.
    void add(const Car& car);

private:
    // Coordinates in the world where the road 'starts'.
    sf::Vector2f m_start;
    // Coordinates in the world where the road 'ends'.
    sf::Vector2f m_end;
    // Unit vector in the direction the road points.
    sf::Vector2f m_direction;
    // Vector of all cars currently on the given road.
    std::vector<Car> m_cars;
    // The physical road shape to be drawn on screen.
    sf::VertexArray m_model;
    // Length of the road (end - start).
    float m_length;
};
