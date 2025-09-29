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
    constexpr unsigned int height = 1000;
    sf::RenderWindow window{ sf::VideoMode{ { width, height } }, "Traffic Simulator" };

    // Create junction
    Junction junction({600.f, 600.f});

    // Create roads
    Road road1({100.f, 100.f}, {600.f, 600.f});
    Road road2({600.f, 600.f}, {1000.f, 300.f});
    Road road3({600.f, 600.f}, {1000.f, 800.f});

    // Connect roads to junction
    junction.add_road(&road1);
    junction.add_road(&road2);
    junction.add_road(&road3);

    // Set end junction
    road1.setEndJunction(&junction);
    road2.setEndJunction(nullptr); // dead-end
    road3.setEndJunction(nullptr);

    // Add car to first road
    auto car1 = std::make_unique<Car>(&road1);
    road1.add(std::move(car1));


    sf::Clock stopwatch;
    while (window.isOpen())
    {
        while (const std::optional<sf::Event> event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }
        window.clear(sf::Color::Black);

        road1.update(stopwatch.getElapsedTime());
        road2.update(stopwatch.getElapsedTime());
        road3.update(stopwatch.getElapsedTime());
        road1.draw(window);
        road2.draw(window);
        road3.draw(window);

        stopwatch.restart();
        window.display();
    }
}
