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
    explicit Car(const std::weak_ptr<Road>& road,
                 const sf::Texture* texture,
                 sf::Color color = sf::Color::White); // [CHANGED] added explicit + color

    void update(sf::Time elapsed);
    void draw(sf::RenderWindow& window);

    // [ADDED - used by Road::update() to determine if car should be removed]
    bool should_remove() const { return m_reached_destination; }

    // [ADDED - destination setter for user-selected routes]
    void set_destination(std::shared_ptr<Junction> dest) { m_destination = dest; }

private:
    // Road reference and kinematics
    std::weak_ptr<Road> m_road;
    float m_relative_distance = 0.f;
    float m_speed = 0.f;
    float m_acceleration = 0.f;

    // --- IDM parameters ---
    float m_max_speed = 0.f;
    float m_max_acceleration = 0.f;
    float m_brake_deceleration = 0.f;
    float m_time_headway = 0.f; // Driver's preferred time gap to the car in front

    static constexpr float CAR_LENGTH = 30.f;
    sf::Vector2f m_position;

    // Visual representation: either rectangle or sprite
    std::variant<sf::RectangleShape, sf::Sprite> m_visual;

    // [ADDED - target junction for despawning logic]
    std::weak_ptr<Junction> m_destination;

    // [ADDED - tracks whether car reached its destination]
    bool m_reached_destination = false;
};