#include "road.hpp"

#include <SFML/Graphics.hpp>
#include <algorithm>
#include <random>

int main()
{
    std::random_device rd;
    constexpr unsigned int width = 1200;
    constexpr unsigned int height = 700;
    sf::RenderWindow window{ sf::VideoMode{ { width, height } }, "Traffic Simulator" };
    Road road_1{ { 100, 100 }, { 600, 600 } };
    Road road_2{ { 600, 600 }, { 1000, 300 } };
    road_1.add(Car{ &road_1 });


    sf::Clock stopwatch;
    stopwatch.start();
    while (window.isOpen())
    {
        while (const std::optional<sf::Event> event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }
        window.clear(sf::Color::Black);

        road_1.update(stopwatch.getElapsedTime());
        road_2.update(stopwatch.getElapsedTime());
        road_1.draw(window);
        road_2.draw(window);

        stopwatch.restart();
        window.display();
    }

    return 0;
}
