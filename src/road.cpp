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

    // Check end-of-road obstacles ONCE for all cars
    Obstacle end_of_road_obstacle;
    end_of_road_obstacle.position = std::numeric_limits<float>::infinity();
    
    if (auto end_junction = getEndJunction().lock())
    {
        auto light_state = end_junction->get_light_state_for_road(shared_from_this());
        
        // Red/yellow light OR blocked junction = obstacle at road end
        if (light_state != TrafficLight::State::Green || end_junction->is_blocked())
        {
            end_of_road_obstacle.position = m_length;
            end_of_road_obstacle.speed = 0.f;
        }
    }

    // Loop 1: Calculate Accelerations using PROPER IDM
    for (size_t i = 0; i < m_cars.size(); ++i)
    {
        Car* current_car = m_cars[i].get();
        Obstacle obstacle;

        // Step 1: Find the closest obstacle
        if (i > 0) // Car in front
        {
            obstacle = { m_cars[i - 1]->m_relative_distance, m_cars[i - 1]->m_speed };
        }
        else // First car - check end of road
        {
            obstacle = end_of_road_obstacle;
        }
        
        // ALL cars should also check end-of-road obstacles
        // Even if there's a car in front, the red light might be closer!
        if (end_of_road_obstacle.position < obstacle.position)
        {
            obstacle = end_of_road_obstacle;
        }

        // === PROPER IDM IMPLEMENTATION ===
        
        // IDM Parameters (from the car's individual properties)
        const float v = current_car->m_speed;                    // Current speed
        const float v_0 = current_car->m_max_speed;              // Desired speed
        const float a = current_car->m_max_acceleration;         // Max acceleration
        const float b = current_car->m_brake_deceleration;       // Comfortable braking
        const float T = current_car->m_time_headway;             // Desired time headway
        const float s_0 = 10.f;                                  // Minimum gap (jam distance)
        const float delta = 4.0f;                                // Acceleration exponent
        
        // Calculate actual gap to obstacle
        float s = obstacle.position - current_car->m_relative_distance - Car::CAR_LENGTH;
        
        // === HANDLE CRITICAL SITUATIONS ===
        if (s <= 0.0f)
        {
            // EMERGENCY: Cars are overlapping!
            current_car->m_acceleration = -b * 2.0f; // Double braking
            current_car->m_speed *= 0.8f; // Force speed reduction
            continue;
        }
        else if (s < s_0)
        {
            // DANGER: Too close, heavy braking
            float danger_factor = s / s_0; // 0 to 1
            current_car->m_acceleration = -b * (1.5f - 0.5f * danger_factor);
            continue;
        }
        
        // === STANDARD IDM CALCULATION ===
        
        // Calculate velocity difference
        float delta_v = v - obstacle.speed;
        
        // Calculate desired gap s*
        float sqrt_term = std::sqrt(a * b);
        float s_star = s_0 + std::max(0.f, v * T + (v * delta_v) / (2.0f * sqrt_term));
        
        // Free road acceleration term
        float free_road_term = std::pow(v / v_0, delta);
        
        // Interaction term
        float interaction_term = std::pow(s_star / s, 2.0f);
        
        // Final IDM acceleration
        float idm_acceleration = a * (1.0f - free_road_term - interaction_term);
        
        // Clamp to reasonable limits
        current_car->m_acceleration = std::clamp(
            idm_acceleration,
            -b * 1.5f,  // Max braking (with some buffer for emergencies)
            a           // Max acceleration
        );
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