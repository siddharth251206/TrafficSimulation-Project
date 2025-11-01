#include "car.hpp"
#include "app_utility.hpp"
#include "road.hpp"
#include "junction.hpp"
#include <cmath>
#include <cstdint>
#include <iostream>

Car::Car(const std::weak_ptr<Road>& road, const sf::Texture* texture, sf::Color color)
    : m_road(road)
{
    if (auto road_ptr = m_road.lock())
        m_position = road_ptr->get_point_at_distance(0.f);

    // If a texture is provided, construct and configure the sprite (SFML 3)
    if (texture)
    {
        m_visual.emplace<sf::Sprite>(*texture);
        sf::Sprite& m_sprite = std::get<sf::Sprite>(m_visual);
        const sf::FloatRect bounds = m_sprite.getLocalBounds();
        const sf::Vector2f originPoint(bounds.size.x / 2.f, bounds.size.y / 2.f);
        m_sprite.setOrigin(originPoint);
        m_sprite.setScale({ 0.5f, 0.5f });

        m_sprite.setColor(color); // [ADDED - tint sprite for color differentiation]
    }
    else
    {
        m_visual.emplace<sf::RectangleShape>(sf::RectangleShape({ CAR_LENGTH, CAR_LENGTH }));
        sf::RectangleShape& m_model = std::get<sf::RectangleShape>(m_visual);
        m_model.setOrigin({ CAR_LENGTH / 2, CAR_LENGTH / 2 });
        m_model.setFillColor(color); // [ADDED - tint rectangle for color differentiation]
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

    // Basic motion update
    m_relative_distance += (m_speed + 0.5f * m_acceleration * dt) * dt;
    m_speed += m_acceleration * dt;
    if (m_speed < 0.f)
        m_speed = 0.f;

    if (auto road_ptr = m_road.lock())
    {
        const float road_len = road_ptr->getLength();
        if (m_relative_distance >= road_len)
            m_relative_distance = road_len;

        // Update position and orientation
        m_position = road_ptr->get_point_at_distance(m_relative_distance);

        if (sf::Sprite* m_sprite = std::get_if<sf::Sprite>(&m_visual))
        {
            m_sprite->setPosition(m_position);
            const sf::Vector2f direction = road_ptr->get_direction();
            const float angle_rad = std::atan2(direction.y, direction.x);
            m_sprite->setRotation(sf::radians(angle_rad));
        }
        else
        {
            sf::RectangleShape& m_model = std::get<sf::RectangleShape>(m_visual);
            m_model.setPosition(m_position);
        }

        // [ADDED - despawn condition if destination is close]
        if (auto dest = m_destination.lock())
        {
            const sf::Vector2f dest_pos = dest->get_location();
            const float dx = dest_pos.x - m_position.x;
            const float dy = dest_pos.y - m_position.y;
            const float distance = std::sqrt(dx * dx + dy * dy);

            if (distance < 25.0f) // threshold for arrival
            {
                m_reached_destination = true;
                std::cout << "[INFO] Car reached destination (" 
                          << dest_pos.x << ", " << dest_pos.y << ")\n";
            }
        }
    }
}

void Car::draw(sf::RenderWindow& window)
{
    if (sf::Sprite* m_sprite = std::get_if<sf::Sprite>(&m_visual))
        window.draw(*m_sprite);
    else if (sf::RectangleShape* m_model = std::get_if<sf::RectangleShape>(&m_visual))
        window.draw(*m_model);
}