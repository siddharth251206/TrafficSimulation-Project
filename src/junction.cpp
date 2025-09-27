#include "junction.hpp"
#include "app_utility.hpp" // for RNG

void Junction::accept_car(std::unique_ptr<Car> car)
{
    j_car = std::move(car);
}

bool Junction::handle_car_redirection()
{
    if (!j_car || j_road_list.empty())
        return false;

    Road* next_road = nullptr;
    if (j_road_list.size() == 1)
        next_road = j_road_list[0];
    else
    {
        do
        {
            size_t idx = RNG::instance().getIndex(0, j_road_list.size() - 1);
            // size_t idx = 1;
            next_road = j_road_list[idx];
        } while (next_road == j_car->m_road);
    }

    j_car->m_relative_distance = 0.f;        // reset distance
    j_car->m_road = next_road;               // update road
    next_road->add(std::move(j_car));
    return true;
}
