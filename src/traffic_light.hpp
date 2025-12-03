#pragma once
#include "road.hpp"
#include <SFML/System/Time.hpp>
#include <memory>

class TrafficLight
{
public:
    enum class State
    {
        Green,
        Yellow,
        Red
    };

    TrafficLight(std::weak_ptr<Road> road);

    // Controlled manually by the Junction now
    void set_state(State state) { m_state = state; }
    State get_state() const { return m_state; }
    
    // Helper to get traffic density for the Junction
    size_t get_car_count() const;
    
    void draw(sf::RenderWindow& window);
    const std::weak_ptr<Road> get_road() const { return m_road; }

private:
    std::weak_ptr<Road> m_road;
    State m_state;
    sf::CircleShape m_model;
};