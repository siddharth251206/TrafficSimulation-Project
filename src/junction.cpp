#include "junction.hpp"
#include "app_utility.hpp"
#include "road.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <memory>
#include <vector>

Junction::Junction(const sf::Vector2f &location) : j_position(location) {}

void Junction::add_road(const std::shared_ptr<Road> &road)
{
    if (point_in_circle(j_position, 20, road->get_start()))
        j_roads_outgoing.push_back(road);
    else if (point_in_circle(j_position, 20, road->get_end()))
        j_roads_incoming.push_back(road);
}

void Junction::accept_car(std::unique_ptr<Car> car) { j_car_queue.push(std::move(car)); }

void Junction::update(sf::Time elapsed)
{
    // Update all traffic lights
    for (auto &light : j_lights)
        light.update(elapsed);

    if (j_is_occupied)
    {
        j_crossing_timer -= elapsed.asSeconds();
        if (j_crossing_timer <= 0.f)
            j_is_occupied = false; // Junction is free
    }

    if (!j_is_occupied && !j_car_queue.empty())
    {
        j_is_occupied = true;
        j_crossing_timer = CROSSING_DELAY;
        handle_car_redirection();
    }
}

void Junction::handle_car_redirection()
{
    auto in_car = std::move(j_car_queue.front());
    j_car_queue.pop();

    if (j_roads_outgoing.empty())
    {
        in_car.reset();
        return;
    }

    size_t idx = RNG::instance().getIndex(0, j_roads_outgoing.size() - 1);
    const std::shared_ptr<Road> next_road = j_roads_outgoing[idx].lock();

    in_car->m_relative_distance = 0.0F;
    in_car->m_road = next_road;
    next_road->add(std::move(in_car));
}

void Junction::install_light(sf::Time green_time)
{
    for (size_t i{}; i < j_roads_incoming.size(); ++i)
        j_lights.push_back(
            TrafficLight(j_roads_incoming[i], green_time, sf::seconds(static_cast<float>((i == 0) ? 0 : i - 1) * (1.1f) * green_time.asSeconds()),
                         j_roads_incoming.size() - 1,
                         (i == 0) ? TrafficLight::State::Green : TrafficLight::State::Red));
}

TrafficLight::State Junction::get_light_state_for_road(std::weak_ptr<Road> road)
{
    for (TrafficLight it : j_lights)
    {
        if (auto it_ptr = it.get_road().lock())
        {
            if (road.lock() && (*it_ptr == *road.lock())) // OH YEAH! OH YEEEAH! LOCK CEREMONY!
                return it.get_state();
        }
    }
    return TrafficLight::State::Green; // Default to green
}

void Junction::draw(sf::RenderWindow &window)
{
    sf::CircleShape circle(j_radius);
    circle.setOrigin({j_radius, j_radius});
    circle.setPosition(j_position);
    circle.setFillColor(sf::Color::Green);
    window.draw(circle);

    // Draw the lights
    for (auto &light : j_lights)
        light.draw(window);
}