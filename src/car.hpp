
// #pragma once
// #include <SFML/Graphics.hpp>

// class Road;

// class Car
// {
// public:
//     Car(Road* road, float initial_dist = 0.f);
//     void update(sf::Time elapsed);
//     void draw(sf::RenderWindow& window) const;

//     Road* m_road;            // Current road the car is on
//     float m_relative_distance = 0.f;
//     float m_speed = 100.f;    // Reduced speed
//     float m_acceleration = 0.f;

// private:
//     sf::Vector2f m_position;
//     sf::RectangleShape m_model{ {50.f, 50.f} };
// };

#pragma once
#include <SFML/Graphics.hpp>

class Road;

class Car
{
public:
    explicit Car(Road* road);
    void update(sf::Time elapsed);
    void draw(sf::RenderWindow& window) const;

    Road* m_road;            // Current road the car is on
    float m_relative_distance = 0.f;
    float m_speed = 100.f;    // Current speed
    float m_acceleration = 0.f;

private:
    sf::Vector2f m_position;
    sf::RectangleShape m_model{ {50.f, 50.f} };
};