#include "road.hpp"
#include "car.hpp"
#include "junction.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <algorithm>
#include <random>

int main()
{
    constexpr unsigned int width = 1200;
    constexpr unsigned int height = 700;
    sf::RenderWindow window{ sf::VideoMode{ { width, height } }, "Traffic Simulator" };

    // Create junction
    Junction junction({600.f, 600.f});

    // Create roads: two incoming, one outgoing
    Road road1({200.f, 600.f}, {600.f, 600.f}); // Incoming
    Road road2({1000.f, 600.f}, {600.f, 600.f}); // Incoming (reversed direction)
    Road road3({600.f, 600.f}, {500.f, 100.f}); // Outgoing

    // Connect roads to junction
    junction.add_road(&road1);
    junction.add_road(&road2);
    junction.add_road(&road3);

    // Set junctions correctly
    road1.setEndJunction(&junction);
    road2.setEndJunction(&junction);
    road3.setStartJunction(&junction);

    // Add initial car to road1
    auto car1 = std::make_unique<Car>(&road1);
    road1.add(std::move(car1));

    // Spawn logic for additional cars on road1
    // int max_cars = 3; // Total cars to spawn on road1, including the first
    // int spawned = 1;  // First car already added

    // One car on road2
    auto car2 = std::make_unique<Car>(&road2);
    road2.add(std::move(car2));

    sf::Clock stopwatch;
    while (window.isOpen())
    {
        while (const std::optional<sf::Event> event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }
        window.clear(sf::Color::Black);

        sf::Time elapsed = stopwatch.getElapsedTime();
        junction.update(elapsed); // Process junction queue
        road1.update(elapsed);
        road2.update(elapsed);
        road3.update(elapsed);

        // Spawn new car on road1 if conditions met
        // if (spawned < max_cars && (road1.is_empty() || road1.get_rearmost_distance() > Road::SAFE_GAP))
        // {
        //     auto new_car = std::make_unique<Car>(&road1);
        //     road1.add(std::move(new_car));
        //     spawned++;
        // }

        road1.draw(window);
        road2.draw(window);
        road3.draw(window);

        stopwatch.restart();
        window.display();
    }
}