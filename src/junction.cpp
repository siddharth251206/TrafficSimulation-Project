#include "junction.hpp"
#include "app_utility.hpp" // for RNG
#include <vector>

void Junction::accept_car(std::unique_ptr<Car> car)
{
    j_car_queue.push(std::move(car));
}

void Junction::update(sf::Time elapsed)
{
    if (j_is_occupied)
    {
        j_crossing_timer -= elapsed.asSeconds();
        if (j_crossing_timer <= 0.f)
        {
            j_is_occupied = false; // Junction is free
            j_car = nullptr;
        }
    }

    if (!j_is_occupied && !j_car_queue.empty())
    {
        j_car = std::move(j_car_queue.front());
        j_car_queue.pop();
        handle_car_redirection();
        j_is_occupied = true;
        j_crossing_timer = CROSSING_DELAY;
    }
}

void Junction::handle_car_redirection()
{
    if (!j_car || j_road_list.empty())
        return;

    // Collect outgoing roads (start at junction, not the incoming road)
    std::vector<Road*> outgoing_roads;
    for (auto* road : j_road_list)
    {
        if (road->get_start() == j_position && road != j_car->m_road)
            outgoing_roads.push_back(road);
    }

    if (outgoing_roads.empty())
        return; // No outgoing roads, car stays or handle differently

    // Pick random outgoing road
    size_t idx = RNG::instance().getIndex(0, outgoing_roads.size() - 1);
    Road* next_road = outgoing_roads[idx];

    j_car->m_relative_distance = 0.f; // Start at beginning of next road
    j_car->m_road = next_road;        // Update road
    next_road->add(std::move(j_car)); // Transfer ownership
}

// #include "junction.hpp"
// #include "app_utility.hpp" // for RNG

// void Junction::accept_car(std::unique_ptr<Car> car)
// {
//     j_car = std::move(car);
// }

// bool Junction::handle_car_redirection()
// {
//     if (!j_car || j_road_list.empty())
//         return false;

//     Road* next_road = nullptr;
//     if (j_road_list.size() == 1)
//         next_road = j_road_list[0];
//     else
//     {
//         do
//         {
//             size_t idx = RNG::instance().getIndex(0, j_road_list.size() - 1);
//             // size_t idx = 1;
//             next_road = j_road_list[idx];
//         } while (next_road == j_car->m_road);
//     }

//     j_car->m_relative_distance = 0.f;        // reset distance
//     j_car->m_road = next_road;               // update road
//     next_road->add(std::move(j_car));
//     return true;
// }

