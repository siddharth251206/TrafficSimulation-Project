#include "road.hpp"
#include "car.hpp"
#include "junction.hpp"
#include <SFML/Graphics.hpp> 
#include <memory>
#include <algorithm>
#include <random>

int main()
{
    // sf::RenderWindow window(sf::VideoMode(1200, 700), "Traffic Simulator");
    constexpr unsigned int width = 1100;
    constexpr unsigned int height =1000;
    sf::RenderWindow window{ sf::VideoMode{ { width, height } }, "Traffic Simulator" };

    // Create junction
    Junction junction({600.f, 600.f});

    // Create roads
    Road road1({100.f, 600.f}, {600.f, 600.f});
    Road road2({1100.f, 600.f}, {600.f, 600.f});
    Road road3({600.f, 600.f}, {1000.f, 800.f});

    // Connect roads to junction
    junction.add_road(&road1);
    junction.add_road(&road2);
    junction.add_road(&road3);

    // Set end junction
    road1.setEndJunction(&junction);
    road2.setEndJunction(&junction); // dead-end
    // road3.setEndJunction(nullptr);

    road3.setStartJunction(&junction);

    // Add car to first road
    auto car1 = std::make_unique<Car>(&road1, 0.0f);
    road1.add(std::move(car1));

    auto car2 = std::make_unique<Car>(&road2, 0.0f);
    road2.add(std::move(car2));

    // auto car3 = std::make_unique<Car>(&road2, -60.0f);
    // road2.add(std::move(car3));


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
        road1.draw(window);
        road2.draw(window);
        road3.draw(window);

        stopwatch.restart();
        window.display();
    }
}
