#pragma once

#include "car.h"

#include <SFML/graphics.hpp>
#include <vector>

class Car;

class Road
{
public:
    Road(const sf::Vector2f& start, const sf::Vector2f& end);
    [[nodiscard]] const sf::Vector2f& get_direction() const noexcept;
    [[nodiscard]] sf::Vector2f get_point_at_distance(float position) const noexcept;

    void update(sf::Time elapsed);
    void draw(sf::RenderWindow& window) const;
    void add(const Car& car);

private:
    sf::Vector2f m_start;
    sf::Vector2f m_end;
    sf::Vector2f m_direction;
    std::vector<Car> m_cars;
    sf::VertexArray m_model;

    float m_length;
};
