#include "car.hpp"
#include "app_utility.hpp"
#include "road.hpp"
#include <cstdint>
#include <cmath>

Car::Car(const std::weak_ptr<Road>& road, const sf::Texture* texture)
    : m_road(road)
{
    if (auto road_ptr = m_road.lock())
        m_position = road_ptr->get_point_at_distance(0.f);

    // Fallback rectangle styling
    m_model.setOrigin({ 15.f, 15.f });
    m_model.setFillColor(
        sf::Color(
            static_cast<std::uint8_t>(RNG::instance().getFloat(100, 255)),
            static_cast<std::uint8_t>(RNG::instance().getFloat(100, 255)),
            static_cast<std::uint8_t>(RNG::instance().getFloat(100, 255))
        )
    );

    // If a texture is provided, configure the sprite
    if (texture)
    {
        m_sprite.setTexture(*texture);
        const sf::FloatRect bounds = m_sprite.getLocalBounds();
        const sf::Vector2f originPoint(bounds.size.x / 2.f, bounds.size.y / 2.f);
        m_sprite.setOrigin(originPoint);
        m_sprite.setScale({0.8f, 0.8f});
        m_sprite.setPosition(m_position);
    }

    // --- IDM property randomization ---
    m_max_speed = RNG::instance().getFloat(110.f, 160.f);
    m_max_acceleration = RNG::instance().getFloat(40.f, 60.f);
    m_brake_deceleration = RNG::instance().getFloat(80.f, 100.f);
    m_time_headway = RNG::instance().getFloat(1.1f, 2.0f);
}

void Car::update(sf::Time elapsed)
{
    const float dt = elapsed.asSeconds();
    m_speed += m_acceleration * dt;
    if (m_speed < 0.f)
        m_speed = 0.f;

    m_relative_distance += m_speed * dt;

    if (auto road_ptr = m_road.lock())
    {
        m_position = road_ptr->get_point_at_distance(m_relative_distance);
        m_model.setPosition(m_position);

        // If using a sprite, update position and rotation to match road direction
        if (m_sprite.getTexture())
        {
            m_sprite.setPosition(m_position);
            const sf::Vector2f direction = road_ptr->get_direction();
            const float angle_rad = std::atan2(direction.y, direction.x);
            m_sprite.setRotation(sf::degrees(to_degrees(angle_rad)));
        }
    }
}

void Car::draw(sf::RenderWindow& window) const
{
    if (m_sprite.getTexture())
        window.draw(m_sprite);
    else
        window.draw(m_model);
}
