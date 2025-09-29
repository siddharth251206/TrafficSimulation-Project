// #pragma once
// #include "road.hpp"
// #include "car.hpp"
// #include <vector>
// #include <memory>

// class Junction
// {
// public:
//     Junction(const sf::Vector2f& location) : j_position(location) {}

//     void add_road(Road* road) { j_road_list.push_back(road); }

//     void accept_car(std::unique_ptr<Car> car);
//     bool handle_car_redirection();

//     const sf::Vector2f& get_location() const { return j_position; }

// private:
//     sf::Vector2f j_position;
//     std::unique_ptr<Car> j_car = nullptr;
//     std::vector<Road*> j_road_list;
// };

#pragma once
#include "road.hpp"
#include "car.hpp"
#include <vector>
#include <memory>
#include <queue>

class Junction
{
public:
    Junction(const sf::Vector2f& location) : j_position(location) {}

    void add_road(Road* road) { j_road_list.push_back(road); }

    void accept_car(std::unique_ptr<Car> car);
    void update(sf::Time elapsed);
    void handle_car_redirection();

    const sf::Vector2f& get_location() const { return j_position; }

private:
    sf::Vector2f j_position;
    std::unique_ptr<Car> j_car = nullptr;
    std::vector<Road*> j_road_list;
    std::queue<std::unique_ptr<Car>> j_car_queue;
    bool j_is_occupied = false;
    float j_crossing_timer = 0.f;
    static constexpr float CROSSING_DELAY = 0.5f; // Time (seconds) for a car to cross junction
};