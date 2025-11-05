#pragma once
#include "road.hpp"
#include <SFML/Graphics.hpp> // For sf::CircleShape
#include <SFML/System/Time.hpp>

class TrafficLight
{
public:
    enum class State
    {
        Green,
        Yellow,
        Red
    };

    // Constructor now just sets up the model
    explicit TrafficLight(std::weak_ptr<Road> road);

    State get_state() const { return m_state; }
    void set_state(State new_state);
    void draw(sf::RenderWindow& window);
    const std::weak_ptr<Road> get_road() const { return m_road; }

private:
    std::weak_ptr<Road> m_road;
    State m_state = State::Red; // Default to red
    sf::CircleShape m_model;
};