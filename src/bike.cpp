#include "bike.hpp"
#include "app_utility.hpp"
#include "road.hpp"

Bike::Bike(const std::weak_ptr<Road>& road, const sf::Texture* texture)
    : Car(road, texture)
{
    if (sf::Sprite* m_sprite = std::get_if<sf::Sprite>(&m_visual)) {
        m_sprite->setScale({0.0080f, 0.0080f}); // smaller bike texture
    } else if (sf::RectangleShape* m_model = std::get_if<sf::RectangleShape>(&m_visual)) {
        m_model->setSize({CAR_LENGTH * 0.5f, CAR_LENGTH * 0.5f});
        m_model->setFillColor(sf::Color(255, 255, 100));
    }

    // Adjust dynamics for lighter bike
    m_max_speed *= 1.2f;
    m_max_acceleration *= 1.3f;
    m_brake_deceleration *= 1.1f;
}
