#include "car.hpp"
#include "road.hpp"

Car::Car(Road* road) : m_position(road->get_point_at_distance(0)), m_road(road)
{
    m_model.setOrigin({ 25.f, 25.f });
}
void Car::update(const sf::Time elapsed)
{
    const float time = elapsed.asSeconds();
    if (m_relative_distance >= m_road->getLength())
    {
        m_relative_distance = m_road->getLength();
        Junction* receive_junction = m_road->getEndJunction();

        if (receive_junction)
        {
            receive_junction->accept_car(this);
            receive_junction->handle_car_redirection();
        }
        else
        {
            m_speed = 0;
        }
    }
    m_relative_distance += (m_speed + 0.5F * m_acceleration * time) * time;
    m_speed += m_acceleration * time;
    m_position = m_road->get_point_at_distance(m_relative_distance);
    m_model.setPosition(m_position);
}

void Car::draw(sf::RenderWindow& window) const { window.draw(m_model); }
