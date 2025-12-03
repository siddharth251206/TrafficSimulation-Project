#include "junction.hpp"
#include "app_utility.hpp"
#include "road.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>

Junction::Junction(const sf::Vector2f& location) : j_position(location) {}

void Junction::add_road(const std::shared_ptr<Road>& road)
{
    if (point_in_circle(j_position, 20, road->get_start()))
        j_roads_outgoing.push_back(road);
    else if (point_in_circle(j_position, 20, road->get_end()))
        j_roads_incoming.push_back(road);
}

void Junction::install_lights()
{
    // Create a traffic light for every incoming road
    for (const auto& road_weak : j_roads_incoming)
    {
        j_lights.emplace_back(road_weak);
    }
    
    if (!j_lights.empty())
    {
        m_active_light_index = 0;
        j_lights[0].set_state(TrafficLight::State::Green);
    }
}

void Junction::accept_car(std::unique_ptr<Car> car) { j_car_queue.push(std::move(car)); }

void Junction::update(sf::Time elapsed)
{
    // 1. Handle the physical crossing of cars (existing logic)
    if (j_is_occupied)
    {
        j_crossing_timer -= elapsed.asSeconds();
        if (j_crossing_timer <= 0.f)
            j_is_occupied = false;
    }

    if (!j_is_occupied && !j_car_queue.empty())
    {
        j_is_occupied = true;
        j_crossing_timer = CROSSING_DELAY;
        handle_car_redirection();
    }

    // 2. Run the Smart Traffic Light Logic
    update_traffic_lights_logic(elapsed);
}

void Junction::update_traffic_lights_logic(sf::Time elapsed)
{
    if (j_lights.empty()) return;

    m_light_timer += elapsed.asSeconds();

    // PHASE 1: TRANSITION (Yellow Light Logic)
    if (m_is_switching)
    {
        if (m_light_timer >= YELLOW_TIME)
        {
            // Transition done: Old Green becomes Red
            if (m_active_light_index >= 0 && m_active_light_index < (int)j_lights.size())
            {
                j_lights[m_active_light_index].set_state(TrafficLight::State::Red);
            }

            // FIND NEXT BEST ROAD
            int best_index = -1;
            size_t max_cars = 0;

            // Simple "Longest Queue" algorithm
            for (size_t i = 0; i < j_lights.size(); ++i)
            {
                // Don't pick the one we just turned off immediately if there are other options
                if ((int)i == m_active_light_index && j_lights.size() > 1) continue;

                size_t count = j_lights[i].get_car_count();
                if (count > max_cars)
                {
                    max_cars = count;
                    best_index = (int)i;
                }
            }

            // If no one has cars, or we couldn't decide, just cycle round-robin
            if (best_index == -1)
            {
                best_index = (m_active_light_index + 1) % j_lights.size();
            }

            // Set new Green
            m_active_light_index = best_index;
            j_lights[m_active_light_index].set_state(TrafficLight::State::Green);
            
            m_is_switching = false;
            m_light_timer = 0.0f; // Reset timer for the green phase
        }
        return; // Wait for yellow time to finish
    }

    // PHASE 2: GREEN LIGHT LOGIC (Should we switch?)
    
    // Safety check
    if (m_active_light_index < 0 || m_active_light_index >= (int)j_lights.size())
    {
        m_active_light_index = 0;
        m_light_timer = 0.0f;
    }

    size_t current_density = j_lights[m_active_light_index].get_car_count();
    
    // Check other roads to see if someone is starving
    bool heavy_traffic_elsewhere = false;
    for(size_t i=0; i<j_lights.size(); ++i) {
        if((int)i == m_active_light_index) continue;
        if(j_lights[i].get_car_count() > current_density + 2) { // Heuristic threshold
            heavy_traffic_elsewhere = true;
            break;
        }
    }

    bool should_switch = false;

    // Condition A: Current road is empty, don't waste time on green
    if (current_density == 0 && m_light_timer > MIN_GREEN_TIME) 
    {
         // Only switch if SOMEONE else actually needs it
         for(const auto& l : j_lights) {
             if(l.get_car_count() > 0) {
                 should_switch = true;
                 break;
             }
         }
    }
    
    // Condition B: We have been green for too long and others are waiting
    if (m_light_timer > MAX_GREEN_TIME)
    {
        should_switch = true;
    }

    // Condition C: Significant traffic elsewhere, switch faster (but respect min time)
    if (heavy_traffic_elsewhere && m_light_timer > MIN_GREEN_TIME * 1.5f)
    {
        should_switch = true;
    }

    if (should_switch)
    {
        j_lights[m_active_light_index].set_state(TrafficLight::State::Yellow);
        m_is_switching = true;
        m_light_timer = 0.0f; // Reset timer for yellow phase
    }
}

void Junction::handle_car_redirection()
{
    if (j_car_queue.empty()) return;
    auto car = std::move(j_car_queue.front());
    j_car_queue.pop();

    if (auto next_road = car->get_next_road_in_path().lock())
    {
        car->m_relative_distance = 0.0f;
        car->m_road = next_road;
        next_road->add(std::move(car));
    }
}

TrafficLight::State Junction::get_light_state_for_road(std::weak_ptr<const Road> road) const
{
    for (const auto& light : j_lights)
    {
        if (auto light_road = light.get_road().lock())
        {
            if (auto q_road = road.lock())
            {
                if (*light_road == *q_road)
                    return light.get_state();
            }
        }
    }
    return TrafficLight::State::Green; // Default if no light found
}

void Junction::draw(sf::RenderWindow& window)
{
    sf::CircleShape circle(j_radius);
    circle.setOrigin({ j_radius, j_radius });
    circle.setPosition(j_position);
    circle.setFillColor(sf::Color(50, 50, 50)); 
    circle.setOutlineColor(sf::Color::White);
    circle.setOutlineThickness(1.f);
    window.draw(circle);

    for (auto& light : j_lights)
        light.draw(window);
}

const std::vector<std::weak_ptr<Road>>& Junction::get_outgoing_roads() const
{
    return j_roads_outgoing;
}