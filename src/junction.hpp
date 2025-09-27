#pragma once
#include "road.hpp"
#include "car.hpp"
#include <vector>
#include <memory>

class Junction
{
public:
    Junction(const sf::Vector2f& location) : j_position(location) {}

    void add_road(Road* road) { j_road_list.push_back(road); }

    void accept_car(std::unique_ptr<Car> car);
    bool handle_car_redirection();

    const sf::Vector2f& get_location() const { return j_position; }

private:
    sf::Vector2f j_position;
    std::unique_ptr<Car> j_car = nullptr;
    std::vector<Road*> j_road_list;
};
