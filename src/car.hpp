#pragma once
#include <SFML/Graphics.hpp>
#include <memory>

class Road;

class Car
{
    friend class Road;
public:
    explicit Car(const std::weak_ptr<Road> &road);
    void update(sf::Time elapsed);
    void draw(sf::RenderWindow &window) const;

    std::weak_ptr<Road> m_road;
    float m_relative_distance = 0.f;

private:
    float m_speed = 0.f;
    float m_acceleration = 0.f;

    // --- IDM parameters ---
    float m_max_speed;
    float m_max_acceleration;
    float m_brake_deceleration;
    float m_time_headway; // Driver's preferred time gap to the car in front
    static constexpr float CAR_LENGTH = 30.f;
    sf::Vector2f m_position;
    sf::RectangleShape m_model{{CAR_LENGTH, CAR_LENGTH}};
};
