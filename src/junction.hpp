#pragma once
#include "car.hpp"
#include "traffic_light.hpp"
#include <SFML/System/Time.hpp>
#include <map>
#include <memory>
#include <queue>
#include <vector>

class Road;// Forward declaration

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

    void install_light(sf::Time green_time);
    TrafficLight::State get_light_state_for_road(std::weak_ptr<const Road> road) const;
    const std::vector<std::weak_ptr<Road>>& get_outgoing_roads() const;

private:
    void handle_car_redirection();

    sf::Vector2f j_position;
    std::vector<std::weak_ptr<Road>> j_roads_incoming;
    std::vector<std::weak_ptr<Road>> j_roads_outgoing;
    std::queue<std::unique_ptr<Car>> j_car_queue;
    bool j_is_occupied = false;
    static constexpr float CROSSING_DELAY = 0.5f;// Seconds for a car to cross junction
    float j_crossing_timer = CROSSING_DELAY;
    float j_radius{ 12.f };
    std::vector<TrafficLight> j_lights;
};