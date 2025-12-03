#include "traffic_spawner.hpp"
#include "app_utility.hpp" // For RNG
#include <iostream>

TrafficSpawner::TrafficSpawner(TrafficMap& map, const sf::Texture* car_texture)
    : m_map(map), m_car_texture(car_texture)
{
}

void TrafficSpawner::update(float delta_time)
{
    m_timer += delta_time;

    // Check if we can spawn based on time and capacity
    if (m_timer >= m_spawn_interval)
    {
        if (m_map.get_car_count() < m_max_cars)
        {
            attempt_spawn();
        }
        
        // Reset timer with a little randomness (traffic isn't perfectly robotic)
        float random_offset = RNG::instance().getFloat(-0.2f, 0.2f);
        m_timer = std::max(0.f, random_offset); 
    }
}

void TrafficSpawner::attempt_spawn()
{
    // 1. Pick a random starting road
    auto start_road = m_map.get_random_road();
    if (!start_road) return;

    // 2. Pick a random destination road
    auto end_road = m_map.get_random_road();
    if (!end_road || start_road == end_road) return;

    // 3. Realistic Constraint: Don't spawn short trips.
    // We want cars to travel ACROSS the map to test the junctions.
    float euclidean_dist = std::hypot(
        start_road->get_start().x - end_road->get_start().x,
        start_road->get_start().y - end_road->get_start().y
    );

    // If trip is too short (less than 800 pixels), skip it.
    if (euclidean_dist < 800.f) return;

    // 4. Check if the start road is full
    // (A simple check to prevent stacking cars on top of each other)
    if (start_road->get_car_count() > 3) return; 

    // 5. Calculate Path
    auto start_junc = start_road->getEndJunction().lock();
    auto end_junc = end_road->getEndJunction().lock();

    if (start_junc && end_junc)
    {
        auto path = m_pathfinder.find_path(start_junc, end_junc);

        // Only spawn if a valid path exists
        if (!path.empty())
        {
            // Ensure the path actually leads to the target road
            if (path.back().lock() == end_road) 
            {
                // Create the car
                // Random speed variance for realism (between 0 and 100 starting distance)
                float start_dist = 10.f; 
                auto car = std::make_unique<Car>(
                    start_road, 
                    m_car_texture, 
                    start_dist
                );

                // Assign the path
                // We use 90% of the last road's length as the specific destination point
                float dest_dist = end_road->getLength() * 0.9f;
                car->set_destination(path, end_road, dest_dist);

                // Inject into the simulation
                start_road->add(std::move(car));
            }
        }
    }
}