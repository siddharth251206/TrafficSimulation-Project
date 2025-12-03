#pragma once
#include "car.hpp"
#include "traffic_light.hpp"
#include <SFML/System/Time.hpp>
#include <map>
#include <memory>
#include <queue>
#include <vector>

class Road;

class Junction
{
public:
    explicit Junction(const sf::Vector2f& location);
    ~Junction() = default;

    void add_road(const std::shared_ptr<Road>& road);
    void accept_car(std::unique_ptr<Car> car);
    void update(sf::Time elapsed);
    void draw(sf::RenderWindow& window);

    const sf::Vector2f& get_location() const { return j_position; }
    bool is_blocked() const { return j_is_occupied || !j_car_queue.empty(); }

    void install_lights(); // Simplified: just adds lights to incoming roads
    TrafficLight::State get_light_state_for_road(std::weak_ptr<const Road> road) const;
    const std::vector<std::weak_ptr<Road>>& get_outgoing_roads() const;

private:
    void handle_car_redirection();
    void update_traffic_lights_logic(sf::Time elapsed);

    sf::Vector2f j_position;
    std::vector<std::weak_ptr<Road>> j_roads_incoming;
    std::vector<std::weak_ptr<Road>> j_roads_outgoing;
    
    // Internal Queue
    std::queue<std::unique_ptr<Car>> j_car_queue;
    bool j_is_occupied = false;
    static constexpr float CROSSING_DELAY = 0.5f; 
    float j_crossing_timer = CROSSING_DELAY;
    
    float j_radius{ 12.f };
    std::vector<TrafficLight> j_lights;

    // --- Dynamic Light Control Variables ---
    int  m_active_light_index = -1; // -1 means all red (or initialization)
    bool m_is_switching = false;    // Are we currently in Yellow phase?
    float m_light_timer = 0.0f;
    
    // Constants for dynamic logic
    const float MIN_GREEN_TIME = 2.0f;  // Minimum time a light stays green
    const float MAX_GREEN_TIME = 10.0f; // Max time before forced switch if others waiting
    const float YELLOW_TIME = 2.0f;
};