#include "truck.hpp"
#include "app_utility.hpp"
#include "road.hpp"

Truck::Truck(const std::weak_ptr<Road>& road, const sf::Texture* texture)
    : Car(road, texture) // Call the base Car constructor
{
    // Check if we are using a sprite or a fallback rectangle
    if (sf::Sprite* m_sprite = std::get_if<sf::Sprite>(&m_visual)) 
    {
        // Make the truck sprite larger than the car sprite
        m_sprite->setScale({0.5f, 0.5f}); 
    } 
    else if (sf::RectangleShape* m_model = std::get_if<sf::RectangleShape>(&m_visual)) 
    {
        // Make the fallback rectangle longer and wider
        m_model->setSize({CAR_LENGTH * 1.8f, CAR_LENGTH * 1.2f});
        m_model->setOrigin({ (CAR_LENGTH * 1.8f) / 2.f, (CAR_LENGTH * 1.2f) / 2.f });
        m_model->setFillColor(sf::Color(100, 100, 255)); // Blue color
    }

    // Adjust dynamics for a heavy truck (slower, less acceleration)
    // These properties were set by the base Car, we are just modifying them.
    m_max_speed *= 0.7f; // 70% of a car's max speed
    m_max_acceleration *= 0.5f; // 50% of a car's acceleration
    m_brake_deceleration *= 0.8f; // Less effective brakes
}
