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
    // Car's internal lifecycle state machine
    enum class CarState
    {
        Spawning,
        Driving,
        Despawning
    };

    static constexpr sf::Time FADE_IN_DURATION = sf::seconds(2.0f);
    static constexpr sf::Time FADE_OUT_DURATION = sf::seconds(0.5f);

    // Supports optional sprite texture; falls back to a rectangle if no texture provided
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
    // True if car has reached its destination
    [[nodiscard]] bool is_finished() const;
    // True if car is finished and is also completely faded-out
    [[nodiscard]] bool is_ready_for_removal() const;

private:
    // Yes, this sets the alpha channel (transparency) of car
    void set_alpha(std::uint8_t alpha);

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

    // State management
    CarState m_state;
    sf::Time m_fade_timer;
};
