#pragma once

#include "car.hpp"
#include <memory>
#include <queue>
#include <vector>

// Forward declaration
class Road;

class Junction
{
public:
    explicit Junction(const sf::Vector2f& location) : j_position(location) {}

    void add_road(const std::shared_ptr<Road>& road);

    void accept_car(std::unique_ptr<Car> car);
    void update(sf::Time elapsed);
    void handle_car_redirection();

    const sf::Vector2f& get_location() const { return j_position; }
    bool is_blocked() const { return j_is_occupied || !j_car_queue.empty(); }

private:
    sf::Vector2f j_position;
    std::unique_ptr<Car> j_car = nullptr;
    std::vector<std::weak_ptr<Road>> j_road_list;
    std::queue<std::unique_ptr<Car>> j_car_queue;
    bool j_is_occupied = false;
    float j_crossing_timer = 0.f;
    static constexpr float CROSSING_DELAY = 0.5f;// Seconds for a car to cross junction
};
