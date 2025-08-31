#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System/Time.hpp>

class Road;
class Junction;

// Represents a car vehicle.
class Car
{
    friend class Junction;
public:
    // Create a car and make it belong to a particular road.
    explicit Car(Road* road);
    // Updates the internal details of the car.
    void update(sf::Time elapsed);
    // Draws the car shape onto the world.
    void draw(sf::RenderWindow& window) const;

private:
    // Coordinates of the car in the world.
    sf::Vector2f m_position;
    // Road that the car belongs to.
    Road* m_road;
    // Distance of the car in the road from the start of the road.
    float m_relative_distance = 0;
    // Speed of the car.
    float m_speed = 1000;
    // Acceleration of the car.
    float m_acceleration = 0;
    // Physical rectangular shape of the car.
    sf::RectangleShape m_model{ { 50.0F, 50.0F } };
};
