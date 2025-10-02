#include <SFML/Graphics.hpp>
#include <memory>
#include "car.hpp"
#include "road.hpp"

Car::Car(std::weak_ptr<Road> road) : m_road(road)
{
    if(auto road_ptr = road.lock())
        m_position = road_ptr->get_point_at_distance(0.f);
    m_model.setOrigin({25.f, 25.f});
    m_model.setFillColor(sf::Color::Blue); // Set visible color
}

void Car::update(sf::Time elapsed)
{
    float dt = elapsed.asSeconds();
    m_relative_distance += (m_speed + 0.5f * m_acceleration * dt) * dt;
    m_speed += m_acceleration * dt;

    // Position update
    if(auto road_ptr = m_road.lock())
        m_position = road_ptr->get_point_at_distance(m_relative_distance);

    m_model.setPosition(m_position);
}

void Car::draw(sf::RenderWindow& window) const
{
    window.draw(m_model);
}