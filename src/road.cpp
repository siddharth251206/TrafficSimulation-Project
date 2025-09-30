#include "road.hpp"
#include "junction.hpp"
#include <SFML/Graphics.hpp>
#include <cmath>
#include <memory>
#include <algorithm>
#include <limits>
#include <vector>

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
        m_direction = diff / m_length;
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

float Road::get_rearmost_distance() const
{
    if (m_cars.empty())
        return std::numeric_limits<float>::infinity();

    float min_dist = m_cars[0]->m_relative_distance;
    for (const auto& c : m_cars)
        min_dist = std::min(min_dist, c->m_relative_distance);

    return min_dist;
}

void Road::update(sf::Time elapsed)
{
    // Sort cars by descending relative distance (leaders first)
    std::vector<Car*> sorted_cars;
    for (auto& c : m_cars)
        sorted_cars.push_back(c.get());

    std::sort(sorted_cars.begin(), sorted_cars.end(), [](const Car* a, const Car* b) {
        return a->m_relative_distance > b->m_relative_distance;
    });

    // Determine if end is blocked
    bool end_blocked = false;
    if (auto* j = getEndJunction())
        end_blocked = j->is_blocked();

    // Calculate new speeds for each car
    std::vector<float> new_speeds(sorted_cars.size(), 100.f);
    for (size_t i = 0; i < sorted_cars.size(); ++i)
    {
        Car* car = sorted_cars[i];
        float leader_dist = std::numeric_limits<float>::infinity();
        float leader_speed = 100.f;

        // Consider road end as potential leader
        if (end_blocked)
        {
            leader_dist = m_length;
            leader_speed = 0.f;
        }

        // If there's a leader car ahead
        if (i > 0)
        {
            Car* leader_car = sorted_cars[i - 1];
            float car_to_leader_dist = leader_car->m_relative_distance - car->m_relative_distance;
            if (car_to_leader_dist < leader_dist - car->m_relative_distance)
            {
                leader_dist = leader_car->m_relative_distance;
                leader_speed = leader_car->m_speed;
            }
        }

        float dist_to_leader = leader_dist - car->m_relative_distance;
        if (dist_to_leader < SAFE_GAP)
            new_speeds[i] = leader_speed;
    }

    // Apply new speeds
    for (size_t i = 0; i < sorted_cars.size(); ++i)
        sorted_cars[i]->m_speed = new_speeds[i];

    // Update positions
    for (auto& car : m_cars)
        car->update(elapsed);

    // Check for cars reaching the end
    for (auto it = m_cars.begin(); it != m_cars.end();)
    {
        if ((*it)->m_relative_distance >= m_length)
        {
            (*it)->m_relative_distance = m_length;
            if (auto* j = getEndJunction())
            {
                j->accept_car(std::move(*it));
                it = m_cars.erase(it);
            }
            else
            {
                (*it)->m_speed = 0.f;
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
