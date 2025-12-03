#include "traffic_light.hpp"
#include <SFML/Graphics.hpp>

TrafficLight::TrafficLight(std::weak_ptr<Road> road)
    : m_road(road), m_state(State::Red), m_model(sf::CircleShape(8.f))
{
    m_model.setOrigin({ 8.f, 8.f });
}

size_t TrafficLight::get_car_count() const
{
    if (auto r = m_road.lock())
        return r->get_car_count();
    return 0;
}

void TrafficLight::draw(sf::RenderWindow& window)
{
    switch (m_state)
    {
    case State::Green:  m_model.setFillColor(sf::Color::Green); break;
    case State::Yellow: m_model.setFillColor(sf::Color::Yellow); break;
    case State::Red:    m_model.setFillColor(sf::Color::Red); break;
    }

    if (auto road = m_road.lock())
    {
        // Position light near the end of the road
        sf::Vector2f light_pos = road->get_point_at_distance(road->getLength() - 20.f);
        m_model.setPosition(light_pos);
        window.draw(m_model);
    }
}