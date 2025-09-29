// #include "car.hpp"
// #include "road.hpp"
// #include "junction.hpp"

// Car::Car(Road* road) : m_road(road), m_position(road->get_point_at_distance(0.f))
// {
//     m_model.setOrigin({25.f, 25.f});
//     m_model.setFillColor(sf::Color::Blue);
// }

// void Car::update(sf::Time elapsed)
// {
//     float dt = elapsed.asSeconds();
//     m_relative_distance += (m_speed + 0.5f * m_acceleration * dt) * dt;
//     m_speed += m_acceleration * dt;


//     m_position = m_road->get_point_at_distance(m_relative_distance);
//     m_model.setPosition(m_position);
// }

// void Car::draw(sf::RenderWindow& window) const
// {
//     window.draw(m_model);
// }

#include "car.hpp"
#include "road.hpp"
#include "junction.hpp"

Car::Car(Road* road, float initial_dist) : m_road(road), m_relative_distance(initial_dist), m_position(road->get_point_at_distance(initial_dist))
{
    m_model.setOrigin({25.f, 25.f});
    m_model.setFillColor(sf::Color::Blue); // Set visible color
}

void Car::update(sf::Time elapsed)
{
    float dt = elapsed.asSeconds();
    m_relative_distance += (m_speed + 0.5f * m_acceleration * dt) * dt;
    m_speed += m_acceleration * dt;

    // Position update
    m_position = m_road->get_point_at_distance(m_relative_distance);
    m_model.setPosition(m_position);
}

void Car::draw(sf::RenderWindow& window) const
{
    window.draw(m_model);
}