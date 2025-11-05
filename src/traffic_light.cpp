#include "traffic_light.hpp"
#include <SFML/Graphics.hpp>

TrafficLight::TrafficLight(std::weak_ptr<Road> road)
    : m_road(road), m_state(State::Red), m_model(sf::CircleShape(8.f))
{
    m_model.setOrigin({ 8.f, 8.f });
    // Set initial color
    m_model.setFillColor(sf::Color::Red);
}

void TrafficLight::set_state(State new_state)
{
    m_state = new_state;
    switch (m_state)
    {
    case TrafficLight::State::Green:
        m_model.setFillColor(sf::Color::Green);
        break;
    case TrafficLight::State::Yellow:
        m_model.setFillColor(sf::Color::Yellow);
        break;
    case TrafficLight::State::Red:
        m_model.setFillColor(sf::Color::Red);
        break;
    }
}

void TrafficLight::draw(sf::RenderWindow& window)
{
    // Draw logic remains the same
    if (auto road = m_road.lock())
    {
        sf::Vector2f light_pos = road->get_point_at_distance(road->getLength() - 20.f);
        m_model.setPosition(light_pos);
        window.draw(m_model);
    }
}