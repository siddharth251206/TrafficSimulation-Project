#include "car.hpp"
#include "traffic_map.hpp"
#include <SFML/Graphics.hpp>
#include <memory>

int main()
{
    constexpr unsigned int width = 1200;
    constexpr unsigned int height = 700;
    sf::RenderWindow window{ sf::VideoMode{ { width, height } }, "Traffic Simulator" };
    window.setFramerateLimit(60);

    TrafficMap traffic_map;

    traffic_map.add_road({ 200.f, 600.f }, { 600.f, 600.f });
    traffic_map.add_road({ 1000.f, 600.f }, { 600.f, 600.f });
    traffic_map.add_road({ 600.f, 600.f }, { 500.f, 100.f });
    traffic_map.add_road({ 500.f, 100.f }, { 200.f, 600.f });
    traffic_map.add_road({ 500.f, 100.f }, { 1000.f, 600.f });

    for (size_t i{}; auto current_road = traffic_map.get_road(i).lock(); i++)
    {
        if (i == 4)
            continue;
        current_road->add(std::make_unique<Car>(current_road));
    }


    sf::Clock clock;
    while (window.isOpen())
    {
        while (const std::optional<sf::Event> event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }
        sf::Time elapsed = clock.restart();

        traffic_map.update(elapsed);

        window.clear(sf::Color::Black);
        traffic_map.draw(window);
        window.display();
    }
    return 0;
}
