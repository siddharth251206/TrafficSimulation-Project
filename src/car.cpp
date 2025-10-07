#include "car.hpp"
#include "app_utility.hpp"
#include "road.hpp"
#include <cstdint>

Car::Car(const std::weak_ptr<Road>& road) : m_road(road)
{
    if (auto road_ptr = road.lock())
        m_position = road_ptr->get_point_at_distance(0.f);

    m_model.setOrigin({ 15.f, 15.f });
    m_model.setFillColor(
        sf::Color(
            static_cast<std::uint8_t>(RNG::instance().getFloat(100, 255)),
            static_cast<std::uint8_t>(RNG::instance().getFloat(100, 255)),
            static_cast<std::uint8_t>(RNG::instance().getFloat(100, 255))
        )
    );

    // --- IDM property randomization ---
    m_max_speed = RNG::instance().getFloat(110.f, 160.f);
    m_max_acceleration = RNG::instance().getFloat(40.f, 60.f);
    m_brake_deceleration = RNG::instance().getFloat(80.f, 100.f);
    m_time_headway = RNG::instance().getFloat(1.1f, 2.0f);
}

void Car::update(sf::Time elapsed)
{
    float dt = elapsed.asSeconds();
    m_speed += m_acceleration * dt;
    if (m_speed < 0.f)
        m_speed = 0.f;

    m_relative_distance += m_speed * dt;

    if (auto road_ptr = m_road.lock())
        m_position = road_ptr->get_point_at_distance(m_relative_distance);
    m_model.setPosition(m_position);
}

void Car::draw(sf::RenderWindow& window) const { window.draw(m_model); }