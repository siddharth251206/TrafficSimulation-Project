#include "road.hpp"
#include "junction.hpp"
#include <utility>
#include <SFML/Graphics.hpp>   // For sf::VertexArray, sf::Vertex
#include <cmath>               // For sqrt in direction calculation
#include <memory>              // For unique_ptr


Road::Road(const sf::Vector2f& start, const sf::Vector2f& end)
    : m_start(start), m_end(end),
      m_model(sf::PrimitiveType::Lines, 2)
{
    // Compute vector from start to end
    sf::Vector2f diff = end - start;

    // Compute length
    m_length = std::sqrt(diff.x * diff.x + diff.y * diff.y);

    // Normalize direction
    if (m_length != 0)
        m_direction = diff / m_length; // now safe because m_length is float
    else
        m_direction = {0.f, 0.f};

    // Set vertices for drawing
    m_model[0] = sf::Vertex{ m_start };
    m_model[1] = sf::Vertex{ m_end };
}

void Road::add(std::unique_ptr<Car> car)
{
    m_cars.push_back(std::move(car));
}

void Road::update(sf::Time elapsed)
{
    for (auto it = m_cars.begin(); it != m_cars.end();)
    {
        (*it)->update(elapsed);

        if ((*it)->m_relative_distance >= m_length)
        {
            (*it)->m_relative_distance = m_length;
            Junction* end_junction = m_junctions.second;
            if (end_junction)
            {
                end_junction->accept_car(std::move(*it)); // Transfer ownership
                end_junction->handle_car_redirection();   // Move to next road
                it = m_cars.erase(it);                    // Remove from this road
            }
            else
            {
                (*it)->m_speed = 0.f; // Dead-end: stop the car
                ++it;
            }
        }
        else
        {
            ++it;
        }
    }
}

void Road::draw(sf::RenderWindow& window) const
{
    window.draw(m_model);
    for (const auto& car : m_cars)
        car->draw(window);
}
