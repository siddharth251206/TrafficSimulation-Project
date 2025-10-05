#include "traffic_light.hpp"
#include <SFML/Graphics.hpp>

TrafficLight::TrafficLight(std::weak_ptr<Road> road, sf::Time green_duration, sf::Time init_time, size_t road_count, State initial_state)
    : m_road(road), m_green_duration(green_duration), m_state(initial_state), m_timer(init_time), m_model(sf::CircleShape(8.f)), adjacent_road_count(road_count)
{
    m_model.setOrigin({8.f, 8.f});
}

void TrafficLight::update(sf::Time elapsed)
{
    m_timer += elapsed;

    switch (m_state)
    {
    case State::Green:
        if (m_timer >= m_green_duration)
        {
            m_state = State::Yellow;
            m_timer = sf::Time::Zero;
        }
        break;
    case State::Yellow:
        if (m_timer >= sf::seconds(m_green_duration.asSeconds() / 10))
        {
            m_state = State::Red;
            m_timer = sf::Time::Zero;
        }
        break;
    case State::Red:
        if (m_timer >= sf::seconds((1.1f * static_cast<float>(adjacent_road_count)) * m_green_duration.asSeconds()))
        {
            m_state = State::Green;
            m_timer = sf::Time::Zero;
        }
        break;
    }
}

void TrafficLight::draw(sf::RenderWindow &window)
{
    switch (get_state())
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
    if (auto road = m_road.lock())
    {
        sf::Vector2f light_pos = road->get_point_at_distance(road->getLength() - 20.f);
        m_model.setPosition(light_pos);
        window.draw(m_model);
    }
}