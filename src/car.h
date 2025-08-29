#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System/Time.hpp>

class Road;

class Car
{
public:
    Car() = default;
    explicit Car(Road* road);
    void update(sf::Time elapsed);
    void draw(sf::RenderWindow& window) const;

private:
    sf::Vector2f m_position;
    Road* m_road;
    float m_relative_distance = 0;
    float m_speed = 1000;
    float m_acceleration = -1000;
    sf::RectangleShape m_model{{ 50.0F, 50.0F }};
};
