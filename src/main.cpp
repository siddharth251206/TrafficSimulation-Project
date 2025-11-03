#include "camera.hpp"
#include "traffic_light.hpp"
#include "traffic_map.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <memory>
#include <optional>
#include <string>

int main()
{
    unsigned int width = 1200;
    unsigned int height = 800;
    sf::RenderWindow window{ sf::VideoMode{ { width, height } }, "Traffic Simulator" };
    window.setFramerateLimit(60);

    // Initialize traffic map
    TrafficMap traffic_map;

    // ------ SVNIT MAP ------
    const sf::Vector2f gajjar_junction = { 700.f, 100.f };
    const sf::Vector2f bhabha_junction = { 200.f, 100.f };
    const sf::Vector2f nehru_junction = { 100.f, 100.f };
    const sf::Vector2f swami_junction = { 100.f, 200.f };
    const sf::Vector2f swami_entrance_junction = { 200.f, 200.f };
    const sf::Vector2f quarters_junction = { 700.f, 375.f };
    const sf::Vector2f canteen_junction = { 200.f, 375.f };
    const sf::Vector2f temple_junction = { 900.f, 300.f };
    const sf::Vector2f gate1_junction = { 200.f, 700.f };
    const sf::Vector2f gate2_junction = { 700.f, 700.f };
    const sf::Vector2f library_junction = { 300.f, 375.f };
    const sf::Vector2f admin_junction = { 300.f, 500.f };
    const sf::Vector2f department_junction = { 700.f, 500.f };

    // Add roads to the traffic map
    traffic_map.add_double_road(gate1_junction, { 200.f, 465.f }, 24.f, false);
    traffic_map.add_double_road(bhabha_junction, gajjar_junction, 24.f);
    traffic_map.add_double_road(nehru_junction, bhabha_junction, 24.f);
    traffic_map.add_double_road(nehru_junction, swami_junction, 24.f);
    traffic_map.add_double_road(bhabha_junction, swami_entrance_junction, 24.f);
    traffic_map.add_double_road(swami_junction, swami_entrance_junction, 24.f);
    traffic_map.add_double_road(swami_entrance_junction, { 200.f, 300.f }, 24.f);
    traffic_map.add_double_road({ 200.f, 300.f }, { 700.f, 300.f }, 24.f);
    traffic_map.add_double_road(gajjar_junction, { 700.f, 300.f }, 24.f);
    traffic_map.add_double_road({ 200.f, 300.f }, canteen_junction, 24.f);
    traffic_map.add_double_road(canteen_junction, quarters_junction, 24.f);
    traffic_map.add_double_road({ 700.f, 300.f }, quarters_junction, 24.f);
    traffic_map.add_double_road({ 700.f, 300.f }, temple_junction, 24.f);
    traffic_map.add_double_road(quarters_junction, { 900.f, 375.f }, 24.f);
    traffic_map.add_double_road(gajjar_junction, { 900.f, 100.f }, 24.f);
    traffic_map.add_double_road({ 900.f, 100.f }, temple_junction, 24.f);
    traffic_map.add_double_road({ 900.f, 375.f }, temple_junction, 24.f);
    traffic_map.add_double_road({ 100.f, 375.f }, canteen_junction, 24.f);
    traffic_map.add_double_road(canteen_junction, { 200.f, 465.f }, 24.f);
    traffic_map.add_double_road({ 100.f, 465.f }, { 200.f, 465.f }, 24.f);
    traffic_map.add_double_road({ 100.f, 465.f }, { 100.f, 375.f }, 24.f);
    traffic_map.add_double_road({ 100.f, 465.f }, { 100.f, 700.f }, 24.f);
    traffic_map.add_double_road({ 100.f, 700.f }, gate1_junction, 24.f);
    traffic_map.add_double_road(gate1_junction, gate2_junction, 24.f);
    traffic_map.add_double_road(library_junction, admin_junction, 24.f);
    traffic_map.add_double_road(admin_junction, { 300.f, 700.f }, 24.f);
    traffic_map.add_double_road(department_junction, gate2_junction, 24.f);
    traffic_map.add_double_road(department_junction, quarters_junction, 24.f);
    traffic_map.add_double_road(department_junction, admin_junction, 24.f);


    // Load car texture
    sf::Texture car_texture;
    bool loaded = false;
    loaded = AssetHelper::try_load_texture(car_texture, "assets/premium_car.png", "premium car");
    if (!loaded)
        loaded =
            AssetHelper::try_load_texture(car_texture, "assets/advanced_car.png", "advanced car");
    if (!loaded)
        loaded = AssetHelper::try_load_texture(car_texture, "assets/car.png", "simple car");

    // Car spawner logic
    sf::Clock spawn_timer;
    int max_cars = 5;
    int spawned_count = 0;

    CameraController camera_controller(static_cast<float>(width), static_cast<float>(height));

    sf::Clock clock;
    while (window.isOpen())
    {
        sf::Time elapsed = clock.restart();
        float deltaTime = elapsed.asSeconds();

        // Handle events
        while (const std::optional<sf::Event> event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (const auto* wrz = event->getIf<sf::Event::Resized>())
            {
                width = wrz->size.x;
                height = wrz->size.y;
            }

            // Smooth zooming centered on mouse cursor
            if (const auto* wheel = event->getIf<sf::Event::MouseWheelScrolled>())
                camera_controller.handle_zoom(window, wheel, width, height);

            // Handle mouse dragging
            camera_controller.handle_mouse_drag(window, event);
        }

        // Smooth keyboard panning
        camera_controller.handle_kb_panning(deltaTime);

        // Clamp camera to map boundaries with safety checks
        camera_controller.clamp_camera();

        // Spawn cars
        if (spawned_count < max_cars && spawn_timer.getElapsedTime().asSeconds() > 1.5f)
        {
            if (auto road = traffic_map.get_double_road(0))
            {
                road->add_to_forward(
                    std::make_unique<Car>(road->get_forward(), loaded ? &car_texture : nullptr)
                );
                spawned_count++;
            }
            spawn_timer.restart();
        }

        // Update and render
        traffic_map.update(elapsed);
        window.setView(camera_controller.get_camera());
        window.clear(sf::Color(20, 20, 40));
        traffic_map.draw(window);
        window.display();
    }
}
