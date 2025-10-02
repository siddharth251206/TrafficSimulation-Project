#include <SFML/Graphics.hpp>
#include <memory>
#include "car.hpp"
#include "traffic_map.hpp"

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


    if (Road* road1 = traffic_map.get_road(0))
        road1->add(std::make_unique<Car>(road1));
    if (Road* road2 = traffic_map.get_road(1))
        road2->add(std::make_unique<Car>(road2));
    if (Road* road3 = traffic_map.get_road(2))
        road3->add(std::make_unique<Car>(road3));
    if (Road* road4 = traffic_map.get_road(3))
        road4->add(std::make_unique<Car>(road4));


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
}
