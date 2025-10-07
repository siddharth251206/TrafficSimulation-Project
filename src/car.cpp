#include "car.hpp"
#include "app_utility.hpp"
#include "road.hpp"
#include <cstdint>
#include <cmath>
#include <iostream>

sf::Font& Car::getFont() {
    static sf::Font font;
    static bool loaded = false;
    if (!loaded) {
        if (font.openFromFile("assets/Arial.ttf")) {
            loaded = true;
        } else {
            std::cerr << "Error: Failed to load font assets/Arial.ttf" << std::endl;
        }
    }
    return font;
}

Car::Car(const std::weak_ptr<Road>& road) : m_road(road),
      m_speed_text(getFont(), "", 14)
{
     m_speed_text.setFillColor(sf::Color::White);
    if (auto road_ptr = road.lock()) {
        m_position = road_ptr->get_point_at_distance(0.f);
        // Add rotation logic for the car model
        const sf::Vector2f& direction = road_ptr->get_direction();
        float angle_rad = std::atan2(direction.y, direction.x);
        float angle_deg = angle_rad * 180.f / 3.14159265f;
        m_model.setRotation(sf::degrees(angle_deg));
    }

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

    m_speed_text.setString(std::to_string(static_cast<int>(m_speed)));
    sf::FloatRect text_bounds = m_speed_text.getLocalBounds();
    m_speed_text.setOrigin({text_bounds.position.x + text_bounds.size.x / 2.0f,
                            text_bounds.position.y + text_bounds.size.y / 2.0f});
    m_speed_text.setPosition({m_position.x, m_position.y - 25.f});
    m_speed_text.setRotation(sf::degrees(0.f)); // Keep text upright
}

void Car::draw(sf::RenderWindow& window) const { window.draw(m_model); 
 window.draw(m_speed_text);}