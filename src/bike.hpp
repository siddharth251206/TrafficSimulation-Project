#ifndef BIKE_HPP
#define BIKE_HPP

#include "car.hpp"

class Bike : public Car {
public:
    explicit Bike(const std::weak_ptr<Road>& road, const sf::Texture* texture = nullptr);
};

#endif
