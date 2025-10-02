#include "car.hpp"
#include "junction.hpp"
#include "app_utility.hpp"
#include "road.hpp"
#include <vector>
#include <memory> // Always include headers for classes you use, irrespective of transitive includes

void Junction::add_road(const std::shared_ptr<Road>& road)
{
    if (road->get_start() == j_position)
        j_roads_outgoing.push_back(road);
    else
        j_roads_incoming.push_back(road);
}

void Junction::accept_car(std::unique_ptr<Car> car) { j_car_queue.push(std::move(car)); }

void Junction::update(sf::Time elapsed)
{
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

void Junction::handle_car_redirection()
{
    auto in_car = std::move(j_car_queue.front());
    j_car_queue.pop();

    if (j_roads_outgoing.empty())
    {
        in_car->m_speed = 0.f;
        return;
    }

    const size_t idx = RNG::instance().getIndex(0, j_roads_outgoing.size() - 1);
    std::shared_ptr<Road> next_road = j_roads_outgoing[idx].lock();

    in_car->m_relative_distance = 0.0F;
    in_car->m_road = next_road;
    next_road->add(std::move(in_car));
}

// =================== Junction_Hash ===================

size_t Junction_Hash::operator()(const sf::Vector2f& j) const noexcept
{
    // A common way to combine hashes for a 2D vector
    size_t h1 = std::hash<float>{}(j.x);
    size_t h2 = std::hash<float>{}(j.y);
    // A good hash combination formula to reduce collisions
    return h1 ^ (h2 << 1);
}
