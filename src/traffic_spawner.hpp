#pragma once
#include "traffic_map.hpp"
#include "pathfinder.hpp"
#include <SFML/Graphics.hpp>
#include <vector>

class TrafficSpawner
{
public:
    TrafficSpawner(TrafficMap& map, const sf::Texture* car_texture);

    // Call this every frame in your main loop
    void update(float delta_time);

    // Configuration
    void set_max_cars(size_t count) { m_max_cars = count; }
    void set_spawn_rate(float seconds_per_car) { m_spawn_interval = seconds_per_car; }

private:
    void attempt_spawn();

    TrafficMap& m_map;
    const sf::Texture* m_car_texture;
    PathFinder m_pathfinder;

    float m_timer = 0.f;
    float m_spawn_interval = 0.8f; // Try to spawn a car every 0.8 seconds
    size_t m_max_cars = 150;
};