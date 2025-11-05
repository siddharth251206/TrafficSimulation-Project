#include "junction.hpp"
#include "app_utility.hpp"
#include "road.hpp"
#include <SFML/Graphics.hpp>
#include <algorithm> // for std::min/max
#include <cmath>     // for std::pow
#include <iostream>
#include <memory>
#include <vector>

Junction::Junction(const sf::Vector2f& location) : j_position(location) {}

void Junction::add_road(const std::shared_ptr<Road>& road)
{
    if (point_in_circle(j_position, 20, road->get_start()))
        j_roads_outgoing.push_back(road);
    else if (point_in_circle(j_position, 20, road->get_end()))
        j_roads_incoming.push_back(road);
}

void Junction::accept_car(std::unique_ptr<Car> car) { j_car_queue.push(std::move(car)); }

void Junction::update(sf::Time elapsed)
{
    // --- THIS IS THE NEW "SMART" UPDATE LOGIC ---
    if (j_has_lights)
    {
        j_cycle_timer += elapsed;

        switch (j_current_cycle_state)
        {
        case TrafficLight::State::Green:
            // Check if green time has expired
            if (j_cycle_timer >= j_current_green_duration)
            {
                // Transition from Green to Yellow
                j_lights[j_current_green_index].set_state(TrafficLight::State::Yellow);
                j_current_cycle_state = TrafficLight::State::Yellow;
                j_cycle_timer = sf::Time::Zero;
            }
            break;

        case TrafficLight::State::Yellow:
            // Check if yellow time has expired
            if (j_cycle_timer >= j_yellow_duration)
            {
                // Transition from Yellow to Red, then to next Green
                
                // 1. Turn the old light red
                j_lights[j_current_green_index].set_state(TrafficLight::State::Red);

                // 2. Advance to the next light in the cycle
                j_current_green_index = (j_current_green_index + 1) % j_lights.size();

                // 3. THIS IS YOUR DYNAMIC LOGIC:
                //    Calculate the *next* green light's duration based on its road's density
                j_current_green_duration =
                    calculate_dynamic_green_time(j_lights[j_current_green_index].get_road());

                // 4. Turn the new light green and reset the cycle
                j_lights[j_current_green_index].set_state(TrafficLight::State::Green);
                j_current_cycle_state = TrafficLight::State::Green;
                j_cycle_timer = sf::Time::Zero;
            }
            break;

        case TrafficLight::State::Red:
            // This state is only for the *initial* setup.
            // The cycle will start itself from install_light().
            break;
        }
    }
    // ---------------------------------------------

    // Car crossing logic remains the same
    if (j_is_occupied)
    {
        j_crossing_timer -= elapsed.asSeconds();
        if (j_crossing_timer <= 0.f)
            j_is_occupied = false;// Junction is free
    }

    if (!j_is_occupied && !j_car_queue.empty())
    {
        j_is_occupied = true;
        j_crossing_timer = CROSSING_DELAY;
        handle_car_redirection();
    }
}

// --- NEW HELPER FUNCTION ---
sf::Time Junction::calculate_dynamic_green_time(const std::weak_ptr<Road>& road)
{
    float density_factor = 1.0f; // Default = base time

    if (auto road_ptr = road.lock())
    {
        size_t car_count = road_ptr->get_car_count();

        // Normalize density from 0.0 to 1.0
        float density_ratio = std::min(
            1.0f,
            static_cast<float>(car_count) / static_cast<float>(MAX_DENSITY_CAR_COUNT)
        );

        // Interpolate between MIN and MAX factors
        // Example: if density_ratio is 0.5 (half full),
        // factor = 0.5 + (0.5 * (1.5 - 0.5)) = 0.5 + (0.5 * 1.0) = 1.0
        density_factor =
            MIN_GREEN_TIME_FACTOR
            + (density_ratio * (MAX_GREEN_TIME_FACTOR - MIN_GREEN_TIME_FACTOR));
    }

    return j_base_green_duration * density_factor;
}
// ---------------------------

void Junction::handle_car_redirection()
{
    if (j_car_queue.empty())
        return;
    auto car = std::move(j_car_queue.front());
    j_car_queue.pop();

    if (auto next_road = car->get_next_road_in_path().lock())
    {
        car->m_relative_distance = 0.0f;
        car->m_road = next_road;
        next_road->add(std::move(car));
    }
}

// --- MODIFIED ---
// This function now just creates the "dumb" lights and
// initializes the junction's *own* state machine.
void Junction::install_light(sf::Time base_green_duration, sf::Time base_yellow_duration)
{
    if (j_roads_incoming.empty())
        return;

    // Create a "dumb" light for each incoming road
    for (const auto& road_weak : j_roads_incoming)
    {
        j_lights.emplace_back(road_weak);
    }

    // Store the base timings
    j_base_green_duration = base_green_duration;
    j_yellow_duration = base_yellow_duration;

    // --- Initialize the state machine ---
    j_has_lights = true;
    j_cycle_timer = sf::Time::Zero;
    j_current_green_index = 0; // Start with the first light

    // Calculate the *first* green light's dynamic duration
    j_current_green_duration =
        calculate_dynamic_green_time(j_lights[j_current_green_index].get_road());

    // Set the first light to green
    j_lights[j_current_green_index].set_state(TrafficLight::State::Green);
    j_current_cycle_state = TrafficLight::State::Green;

    // All other lights are already Red by default.
}
// -----------------

TrafficLight::State Junction::get_light_state_for_road(std::weak_ptr<const Road> road) const
{
    // If a junction has no lights, all roads are green by default.
    if (!j_has_lights)
    {
        return TrafficLight::State::Green;
    }
    
    // Find the light corresponding to this road
    for (const auto& light : j_lights)
    {
        if (auto it_ptr = light.get_road().lock())
        {
            if (auto road_ptr = road.lock())
            {
                if (*it_ptr == *road_ptr)
                {
                    return light.get_state();
                }
            }
        }
    }

    // This case shouldn't be hit if install_light works,
    // but as a safety, default to Red if a light is missing.
    return TrafficLight::State::Red;
}

void Junction::draw(sf::RenderWindow& window)
{
    sf::CircleShape circle(j_radius);
    circle.setOrigin({ j_radius, j_radius });
    circle.setPosition(j_position);
    circle.setFillColor(sf::Color(40, 40, 40)); // Darker junction
    window.draw(circle);

    // Draw the lights
    for (auto& light : j_lights)
        light.draw(window);
}
const std::vector<std::weak_ptr<Road>>& Junction::get_outgoing_roads() const
{
    return j_roads_outgoing;
}