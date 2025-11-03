#pragma once

#include <SFML/Graphics.hpp>
#include <deque>
#include <memory>
#include <variant>

class Road;
class Junction;

class Car
{
    friend class Road;
    friend class Junction;

public:
    // Add color parameter (default white)
    explicit Car(
        const std::weak_ptr<Road>& road,
        const sf::Texture* texture = nullptr,
        float start_distance = 0.f,
        const sf::Color& color = sf::Color::White
    );

    void update(sf::Time elapsed);
    void draw(sf::RenderWindow& window);

    // destination path: deque of roads (weak_ptr), final_road and distance on final road
    void set_destination(
        std::deque<std::weak_ptr<Road>> path,
        const std::weak_ptr<Road>& final_road,
        float destination_distance
    );

    std::weak_ptr<Road> get_next_road_in_path();
    void advance_path();
    [[nodiscard]] bool is_finished() const;

private:
    // Road reference and kinematics
    std::weak_ptr<Road> m_road;
    float m_relative_distance = 0.f;
    float m_speed = 0.f;
    float m_acceleration = 0.f;

    // --- IDM parameters ---
    float m_max_speed;
    float m_max_acceleration;
    float m_brake_deceleration;
    float m_time_headway; // Driver preferred headway

    static constexpr float CAR_LENGTH = 30.f;
    sf::Vector2f m_position;

    // The car visual: either rectangle or sprite
    std::variant<sf::RectangleShape, sf::Sprite> m_visual;

    std::deque<std::weak_ptr<Road>> m_path; // route
    std::weak_ptr<Road> m_final_road;
    float m_destination_distance = 0.f;
    bool m_is_finished = false;
};
