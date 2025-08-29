#include "car.h"
#include "road.h"

Car::Car(Road* road) : m_position(road->get_point_at_distance(0)), m_road(road) {}
void Car::update(const sf::Time elapsed)
{
    const float time = elapsed.asSeconds();
    m_relative_distance += (m_speed + 0.5F * m_acceleration * time) * time;
    m_speed += m_acceleration * time;
    m_position = m_road->get_point_at_distance(m_relative_distance);
    m_model.setPosition(m_position);
}

void Car::draw(sf::RenderWindow& window) const
{
    window.draw(m_model);
}
