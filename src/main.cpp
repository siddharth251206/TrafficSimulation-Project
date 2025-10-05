#include "traffic_map.hpp"
#include "traffic_light.hpp"
#include <SFML/Graphics.hpp>
#include <memory>

int main()
{
    constexpr unsigned int width = 1200;
    constexpr unsigned int height = 800;
    sf::RenderWindow window{ sf::VideoMode{ { width, height } }, "Traffic Simulator" };
    window.setFramerateLimit(60);

    TrafficMap traffic_map;

    // Create a four-way intersection
    const sf::Vector2f center = {600.f, 400.f};
    const sf::Vector2f north = {600.f, 100.f};
    const sf::Vector2f south = {600.f, 700.f};
    const sf::Vector2f east = {1100.f, 400.f};
    const sf::Vector2f west = {100.f, 400.f};

    traffic_map.add_double_road(north, center); // Road 0
    traffic_map.add_double_road(south, center); // Road 1
    traffic_map.add_double_road(west, center);  // Road 2
    traffic_map.add_double_road(east, center);  // Road 3

    // Install traffic lights
    if (auto intersection = traffic_map.get_junction(center)) 
        intersection->install_light(sf::seconds(12)); // North -> Center

    // Car Spawner Logic
    sf::Clock spawn_timer;
    int max_cars = 20;
    int spawned_count = 0;

    sf::Clock clock;
    while (window.isOpen())
    {
        sf::Time elapsed = clock.restart();
        while (const std::optional<sf::Event> event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        // Spawn cars on the north and west roads
        if (spawned_count < max_cars && spawn_timer.getElapsedTime().asSeconds() > 1.5f) {
            if (auto road = traffic_map.get_double_road(0)) { // North road
                road->add_to_forward(std::make_unique<Car>(road->get_forward()));
                spawned_count++;
            }
            if (auto road = traffic_map.get_double_road(2)) { // West road
                road->add_to_forward(std::make_unique<Car>(road->get_forward()));
                spawned_count++;
            }
            spawn_timer.restart();
        }

        traffic_map.update(elapsed);

        window.clear(sf::Color(20, 20, 40));
        traffic_map.draw(window);
        window.display();
    }
}