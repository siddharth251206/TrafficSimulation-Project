#include <SFML/Graphics.hpp>
#include <cmath>
#include <limits>
#include <memory>
#include <vector>
#include "car.hpp"
#include "road.hpp"
#include "junction.hpp"

Road::Road(const sf::Vector2f& start, const sf::Vector2f& end)
    : m_start(start), m_end(end), m_model(sf::PrimitiveType::Lines, 2)
{
    // Compute vector from start to end
    sf::Vector2f diff = end - start;

    // Compute length
    m_length = std::sqrt(diff.x * diff.x + diff.y * diff.y);

    // Normalize direction
    if (m_length != 0)
        m_direction = diff / m_length;
    else
        m_direction = { 0.f, 0.f };

    // Set vertices for drawing
    m_model[0] = sf::Vertex{ m_start };
    m_model[1] = sf::Vertex{ m_end };
}

void Road::add(std::unique_ptr<Car> car) { m_cars.push_back(std::move(car)); }

std::weak_ptr<Junction> Road::getEndJunction() const
{
    return m_junctions.second.lock();
}

void Road::setStartJunction(const std::shared_ptr<Junction>& junction)
{
    m_junctions.first = junction;
}
void Road::setEndJunction(const std::shared_ptr<Junction>& junction)
{
    m_junctions.second = junction;
}

void Road::update(sf::Time elapsed)
{
    constexpr float cruising_speed = 100.0F;
    // An obstacle is either a car ahead or the end of road
    struct Obstacle
    {
        float position = std::numeric_limits<float>::infinity();
        float speed = cruising_speed;
    };

    Obstacle road_end;
    if (std::shared_ptr<Junction> end_junction = getEndJunction().lock();
        end_junction != nullptr && end_junction->is_blocked())
    {
        road_end = { .position = m_length, .speed = 0.0F };
    }

    std::vector<float> target_speeds;
    target_speeds.reserve(m_cars.size());

    for (size_t i = 0; i < m_cars.size(); i++)
    {
        const auto& current_car = m_cars[i];
        Obstacle obstacle = road_end;

        // If there's a car in front, and it's too close -> it becomes the obstacle
        if (i > 0)
        {
            const auto& car_ahead = m_cars[i - 1];
            obstacle = { .position = car_ahead->m_relative_distance, .speed = car_ahead->m_speed };
        }

        // Speed of car = speed of obstacle if too close, else cruising speed
        const float distance_to_obstacle = obstacle.position - current_car->m_relative_distance;
        target_speeds.push_back(distance_to_obstacle < SAFE_GAP ? obstacle.speed : cruising_speed);
    }

    for (size_t i = 0; i < m_cars.size(); i++)
    {
        m_cars[i]->m_speed = target_speeds[i];
        m_cars[i]->update(elapsed);
    }

    // Transfer cars from road to junction (if the conditions are met)
    std::erase_if(
        m_cars,
        [&](std::unique_ptr<Car>& car)
        {
            if (car->m_relative_distance < m_length)
                return false;
            car->m_relative_distance = m_length;

            std::shared_ptr<Junction> end_junction = getEndJunction().lock();
            if (!end_junction)
            {
                car->m_speed = 0.0F;
                return false;
            }

            end_junction->accept_car(std::move(car));
            return true;
        }
    );
}

void Road::draw(sf::RenderWindow& window) const
{
    window.draw(m_model);
    for (const auto& car : m_cars)
        car->draw(window);
}
