#include "road.hpp"
#include "junction.hpp"
#include "traffic_light.hpp"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include <vector>

Road::Road(const sf::Vector2f& start, const sf::Vector2f& end)
    : m_start(start), m_end(end), m_model(sf::PrimitiveType::Lines, 2)
{
    sf::Vector2f diff = end - start;
    m_length = std::sqrt(diff.x * diff.x + diff.y * diff.y);
    if (m_length != 0)
        m_direction = diff / m_length;
    else
        m_direction = { 0.f, 0.f };
    m_model[0] = sf::Vertex{ m_start };
    m_model[1] = sf::Vertex{ m_end };
}
void Road::add(std::unique_ptr<Car> car)
{
    car->advance_path();
    m_cars.push_back(std::move(car));
}

std::weak_ptr<Junction> Road::getEndJunction() const { return m_junctions.second; }

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
    if (m_cars.empty())
        return;

    // Define an obstacle: a car, a red light, or a blocked junction.
    struct Obstacle
    {
        float position = std::numeric_limits<float>::infinity();
        float speed = 0.f;
    };

    // Loop 1: Calculate Accelerations using IDM
    for (size_t i = 0; i < m_cars.size(); ++i)
    {
        Car* current_car = m_cars[i].get();
        Obstacle obstacle;

        // Step 1: Determine the immediate obstacle.
        if (i > 0)// Is there a car in front?
        {
            obstacle = { m_cars[i - 1]->m_relative_distance, m_cars[i - 1]->m_speed };
        }
        else// It is the first car
        {
            // Check for an obstacle at the end of the road (light or blocked junction).
            if (auto end_junction = getEndJunction().lock())
            {
                auto light_state = end_junction->get_light_state_for_road(shared_from_this());
                // A red/yellow light or a backed-up junction is a "wall" at the end of the road.
                if (light_state != TrafficLight::State::Green || end_junction->is_blocked())
                {
                    // The car in front is the primary obstacle.
                    obstacle = { m_length, 0.f };
                }
            }
        }

        // Step 2: Apply the IDM formula with the determined obstacle.
        const float min_jam_distance = 7.f;
        const float acceleration_exponent = 4.0f;

        float s = obstacle.position - current_car->m_relative_distance - Car::CAR_LENGTH;
        if (s < 0.01f)
            s = 0.01f;

        float delta_v = current_car->m_speed - obstacle.speed;

        float s_star =
            min_jam_distance
            + std::max(
                0.f,
                (current_car->m_speed * current_car->m_time_headway
                 + (current_car->m_speed * delta_v)
                       / (2.0f
                          * std::sqrt(
                              current_car->m_max_acceleration * current_car->m_brake_deceleration
                          )))
            );

        float free_road_term =
            std::pow(current_car->m_speed / current_car->m_max_speed, acceleration_exponent);
        float interaction_term = (s_star * s_star) / (s * s);

        // Set the final calculated acceleration for this frame.
        float calculated_acc =
            current_car->m_max_acceleration * (1.0f - free_road_term - interaction_term);

        // this nugget of fuck is why the car's accel isn't negativer than max break force
        // so if this clusterfucking function says, "hi m_acceleration = -100000!"
        // it says, "fuckity nope, m_acceleration is -m_brake_deceleration"
        // and stops cars from disappearing. i do not know why. i am sorry
        current_car->m_acceleration = std::max(-current_car->m_brake_deceleration, calculated_acc);
    }

    // Loop 2: Update car positions based on calculated accelerations
    for (const auto& car : m_cars)
    {
        car->update(elapsed);
    }

    std::erase_if(
        m_cars,
        [&](std::unique_ptr<Car>& car)
        {
            // Despawn cars that have reached their destination
            // This must come before any junction transfer logic
            if (car->is_finished())
                return true;

            // Transfer cars that have reached the end of the road
            if (car->m_relative_distance < m_length)
                return false;
            car->m_relative_distance = m_length;

            if (const std::shared_ptr<Junction> end_junction = getEndJunction().lock())
            {
                end_junction->accept_car(std::move(car));
                return true;
            }
            // Car is stuck at the end of a road with no junction
            car.reset();
            return false;
        }
    );
}

void Road::draw(sf::RenderWindow& window) const
{
    window.draw(m_model);
    for (const auto& car : m_cars)
        car->draw(window);
}

bool Road::operator==(const Road& other) const
{
    return ((m_start == other.m_start) && (m_end == other.m_end));
}
float Road::get_travel_time() const
{
    // Logic for calculating time based on length and m_cars.size() will go here.

    if (auto end_junc = getEndJunction().lock())
    {
        if (end_junc->get_light_state_for_road(shared_from_this()) != TrafficLight::State::Green)
        {
            return std::numeric_limits<float>::infinity();
        }
    }

    constexpr float TYPICAL_SPEED = 100.F;
    const float base_time = m_length / TYPICAL_SPEED;
    constexpr float PENALTY_PER_CAR = 0.25F;
    const float density_penalty = 1.0f + (static_cast<float>(m_cars.size()) * PENALTY_PER_CAR);
    return base_time * density_penalty;
}