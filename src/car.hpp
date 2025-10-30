#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <optional>
#include <variant>

class Road;
class Junction;

class Car
{
    friend class Road;
    friend class Junction;

public:
    // Supports optional sprite texture; falls back to a rectangle if no texture provided
    explicit Car(const std::weak_ptr<Road>& road, const sf::Texture* texture = nullptr);
    // VIRTUAL functions allow child classes (Bike, Truck) to override them if needed
    virtual void update(sf::Time elapsed);
    virtual void draw(sf::RenderWindow& window);
    
    // VIRTUAL destructor is CRITICAL for a base class
    virtual ~Car() = default;

protected:
    // Road reference and kinematics
    std::weak_ptr<Road> m_road;
    float m_relative_distance = 0.f;
    float m_speed = 0.f;
    float m_acceleration = 0.f;

    // --- IDM parameters ---
    float m_max_speed;
    float m_max_acceleration;
    float m_brake_deceleration;
    float m_time_headway;// Driver's preferred time gap to the car in front

    static constexpr float CAR_LENGTH = 30.f;
    sf::Vector2f m_position;
    // The car can be one of rectangle or sprite.
    std::variant<sf::RectangleShape, sf::Sprite> m_visual;
};
