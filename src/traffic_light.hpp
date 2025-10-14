#pragma once
#include "road.hpp"
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

    // Constructor to set the cycle durations and an initial state
    TrafficLight(
        std::weak_ptr<Road> road,
        sf::Time green_duration,
        sf::Time init_time,
        size_t adj_road_count,
        State initial_state
    );

    void update(sf::Time elapsed);
    State get_state() const { return m_state; }
    void draw(sf::RenderWindow& window);
    const std::weak_ptr<Road> get_road() const { return m_road; }

private:
    std::weak_ptr<Road> m_road;

    // How long green state lasts
    sf::Time m_green_duration;
    State m_state;
    sf::Time m_timer;
    sf::CircleShape m_model;
    size_t adjacent_road_count;
};