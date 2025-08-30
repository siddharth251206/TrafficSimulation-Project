#include "road.hpp"

Road::Road(const sf::Vector2f& start, const sf::Vector2f& end)
    : m_start(start), m_end(end), m_direction((end - start).normalized()),
      m_model(sf::PrimitiveType::Lines, 2), m_length((end - start).length())
{
    m_model[0] = sf::Vertex{ m_start };
    m_model[1] = sf::Vertex{ m_end };
}

const sf::Vector2f& Road::get_direction() const noexcept { return m_direction; }

sf::Vector2f Road::get_point_at_distance(const float position) const noexcept
{
    return m_start + position * m_direction;
}

void Road::update(const sf::Time elapsed)
{
    for (auto& car : m_cars)
        car.update(elapsed);
}

void Road::draw(sf::RenderWindow& window) const
{
    window.draw(m_model);
    for (const auto &car : m_cars)
        car.draw(window);
}
void Road::add(const Car& car) { m_cars.push_back(car); }
