#include "road.h"


#include <SFML/Graphics.hpp>
#include <algorithm>

int main()
{
    constexpr unsigned int width = 1200;
    constexpr unsigned int height = 700;
    sf::RenderWindow window{ sf::VideoMode{ { width, height } }, "Traffic Simulator" };
    Road road{ { 100, 100 }, { 600, 600 } };
    road.add(Car{ &road });
    road.add(Car{ &road });

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

        road.update(stopwatch.getElapsedTime());
        road.draw(window);

        stopwatch.restart();
        window.display();
    }

    return 0;
}
