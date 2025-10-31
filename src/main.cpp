
#include "camera.hpp"
#include "traffic_light.hpp"
#include "traffic_map.hpp"
#include "app_utility.hpp"
#include "ux.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>

int main()
{
    unsigned int width = 1200, height = 800;
    sf::RenderWindow window(sf::VideoMode({width, height}), "Traffic Simulator – Source/Dest UI");
    window.setFramerateLimit(60);

    TrafficMap traffic_map;
    // (Add all your junctions and double roads here — unchanged)

    const sf::Vector2f gajjar_junction   = {700.f, 100.f};
    const sf::Vector2f bhabha_junction   = {200.f, 100.f};
    const sf::Vector2f nehru_junction    = {100.f, 100.f};
    const sf::Vector2f swami_junction    = {100.f, 200.f};
    const sf::Vector2f swami_entrance_junction = {200.f, 200.f};
    const sf::Vector2f quarters_junction = {700.f, 375.f};
    const sf::Vector2f canteen_junction  = {200.f, 375.f};
    const sf::Vector2f temple_junction   = {900.f, 300.f};
    const sf::Vector2f gate1_junction    = {200.f, 700.f};
    const sf::Vector2f gate2_junction    = {700.f, 700.f};
    const sf::Vector2f library_junction  = {300.f, 375.f};
    const sf::Vector2f admin_junction    = {300.f, 500.f};
    const sf::Vector2f department_junction = {700.f, 500.f};

    // (all add_double_road calls – unchanged)
    traffic_map.add_double_road(gate1_junction, {200.f, 465.f}, 15.f, false);
    traffic_map.add_double_road(bhabha_junction, gajjar_junction, 15.f);
    traffic_map.add_double_road(nehru_junction, bhabha_junction, 15.f);
    traffic_map.add_double_road(nehru_junction, swami_junction, 15.f);
    traffic_map.add_double_road(bhabha_junction, swami_entrance_junction, 15.f);
    traffic_map.add_double_road(swami_junction, swami_entrance_junction, 15.f);
    traffic_map.add_double_road(swami_entrance_junction, {200.f, 300.f}, 15.f);
    traffic_map.add_double_road({200.f, 300.f}, {700.f, 300.f}, 15.f);
    traffic_map.add_double_road(gajjar_junction, {700.f, 300.f}, 15.f);
    traffic_map.add_double_road({200.f, 300.f}, canteen_junction, 15.f);
    traffic_map.add_double_road(canteen_junction, quarters_junction, 15.f);
    traffic_map.add_double_road({700.f, 300.f}, quarters_junction, 15.f);
    traffic_map.add_double_road({700.f, 300.f}, temple_junction, 15.f);
    traffic_map.add_double_road(quarters_junction, {900.f, 375.f}, 15.f);
    traffic_map.add_double_road(gajjar_junction, {900.f, 100.f}, 15.f);
    traffic_map.add_double_road({900.f, 100.f}, temple_junction, 15.f);
    traffic_map.add_double_road({900.f, 375.f}, temple_junction, 15.f);
    traffic_map.add_double_road({100.f, 375.f}, canteen_junction, 15.f);
    traffic_map.add_double_road(canteen_junction, {200.f, 465.f}, 15.f);
    traffic_map.add_double_road({100.f, 465.f}, {200.f, 465.f}, 15.f);
    traffic_map.add_double_road({100.f, 465.f}, {100.f, 375.f}, 15.f);
    traffic_map.add_double_road({100.f, 465.f}, {100.f, 700.f}, 15.f);
    traffic_map.add_double_road({100.f, 700.f}, gate1_junction, 15.f);
    traffic_map.add_double_road(gate1_junction, gate2_junction, 15.f);
    traffic_map.add_double_road(library_junction, admin_junction, 15.f);
    traffic_map.add_double_road(admin_junction, {300.f, 700.f}, 15.f);
    traffic_map.add_double_road(department_junction, gate2_junction, 15.f);
    traffic_map.add_double_road(department_junction, quarters_junction, 15.f);
    traffic_map.add_double_road(department_junction, admin_junction, 15.f);


    sf::Texture car_texture;
    AssetHelper::try_load_texture(car_texture, "assets/car.png", "car");

    CameraController camera(width, height);

    sf::Font font;
    font.openFromFile("assets/fonts/Roboto-Regular.ttf");

    UXController ux(window, camera, traffic_map, &car_texture, &font);

    sf::Clock clock;

    while (window.isOpen())
    {
        sf::Time elapsed = clock.restart();
        float dt = elapsed.asSeconds();

        while (const std::optional<sf::Event> event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            ux.handle_event(*event);
        }

        ux.update(dt);
        ux.spawn_cars();

        traffic_map.update(elapsed);

        window.setView(camera.get_camera());
        window.clear(sf::Color(20, 20, 40));
        traffic_map.draw(window);

        ux.render_ui();

        window.display();
    }

    return 0;
}
