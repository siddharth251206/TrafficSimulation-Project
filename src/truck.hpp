#ifndef TRUCK_HPP
#define TRUCK_HPP

#include "car.hpp" // Inherits from Car

class Truck : public Car {
public:
    explicit Truck(const std::weak_ptr<Road>& road, const sf::Texture* texture = nullptr);
};

#endif
