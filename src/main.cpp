#include "traffic_light.hpp"
#include "traffic_map.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <memory>
#include <optional>
#include <string>

int main()
{
    constexpr unsigned int width = 1200;
    constexpr unsigned int height = 800;
    sf::RenderWindow window{ sf::VideoMode{ { width, height } }, "Traffic Simulator" };
    window.setFramerateLimit(60);

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

    traffic_map.add_double_road(gate1_junction, { 200.f, 465.f }, 15.f, false);
    traffic_map.add_double_road(bhabha_junction, gajjar_junction, 15.f);
    traffic_map.add_double_road(nehru_junction, bhabha_junction, 15.f);
    traffic_map.add_double_road(nehru_junction, swami_junction, 15.f);
    traffic_map.add_double_road(bhabha_junction, swami_entrance_junction, 15.f);
    traffic_map.add_double_road(swami_junction, swami_entrance_junction, 15.f);
    traffic_map.add_double_road(swami_entrance_junction, { 200.f, 300.f }, 15.f);
    traffic_map.add_double_road({ 200.f, 300.f }, { 700.f, 300.f }, 15.f);
    traffic_map.add_double_road(gajjar_junction, { 700.f, 300.f }, 15.f);

    traffic_map.add_double_road({ 200.f, 300.f }, canteen_junction, 15.f);
    traffic_map.add_double_road(canteen_junction, quarters_junction, 15.f);
    traffic_map.add_double_road({ 700.f, 300.f }, quarters_junction, 15.f);

    traffic_map.add_double_road({ 700.f, 300.f }, temple_junction, 15.f);
    traffic_map.add_double_road(quarters_junction, { 900.f, 375.f }, 15.f);
    traffic_map.add_double_road(gajjar_junction, { 900.f, 100.f }, 15.f);
    traffic_map.add_double_road({ 900.f, 100.f }, temple_junction, 15.f);
    traffic_map.add_double_road({ 900.f, 375.f }, temple_junction, 15.f);

    traffic_map.add_double_road({ 100.f, 375.f }, canteen_junction, 15.f);
    traffic_map.add_double_road(canteen_junction, { 200.f, 465.f }, 15.f);
    traffic_map.add_double_road({ 100.f, 465.f }, { 200.f, 465.f }, 15.f);
    traffic_map.add_double_road({ 100.f, 465.f }, { 100.f, 375.f }, 15.f);
    traffic_map.add_double_road({ 100.f, 465.f }, { 100.f, 700.f }, 15.f);
    traffic_map.add_double_road({ 100.f, 700.f }, gate1_junction, 15.f);
    traffic_map.add_double_road(gate1_junction, gate2_junction, 15.f);

    traffic_map.add_double_road(library_junction, admin_junction, 15.f);
    traffic_map.add_double_road(admin_junction, { 300.f, 700.f }, 15.f);
    traffic_map.add_double_road(department_junction, gate2_junction, 15.f);
    traffic_map.add_double_road(department_junction, quarters_junction, 15.f);
    traffic_map.add_double_road(department_junction, admin_junction, 15.f);

    // // --- Load Car Texture (for sprite-based cars) ---
    sf::Texture car_texture;
    bool loaded = false;
    loaded = AssetHelper::try_load_texture(car_texture, "assets/premium_car.png", "premium car");
    if (!loaded)
        loaded = AssetHelper::try_load_texture(car_texture, "assets/advanced_car.png", "advanced car");
    if (!loaded)
        loaded = AssetHelper::try_load_texture(car_texture, "assets/car.png", "simple car");


    // Car Spawner Logic
    sf::Clock spawn_timer;
    int max_cars = 20;
    int spawned_count = 0;

    sf::View camera(sf::FloatRect(sf::Vector2f{0.f, 0.f}, sf::Vector2f{static_cast<float>(width), static_cast<float>(height)}));
    camera.setCenter(sf::Vector2f(600.f, 400.f)); // Start centered

    float zoomLevel = 1.f; // To track current zoom
    bool dragging = false;
    sf::Vector2i lastMousePos;

    sf::Clock clock;
    while (window.isOpen())
    {
        sf::Time elapsed = clock.restart();
        while (const std::optional<sf::Event> event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            // --- Mouse wheel for zooming ---
            if (const auto* wheel = event->getIf<sf::Event::MouseWheelScrolled>())
            {
                if (wheel->delta > 0)
                    zoomLevel *= 0.9f; // Zoom in
                else
                    zoomLevel *= 1.1f; // Zoom out

                camera.setSize(sf::Vector2f(width * zoomLevel, height * zoomLevel));
            }

            // --- Start or stop dragging ---
            if (const auto* mbp = event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (mbp->button == sf::Mouse::Button::Middle)
                {
                    dragging = true;
                    lastMousePos = sf::Mouse::getPosition(window);
                }
            }
            else if (const auto* mbr = event->getIf<sf::Event::MouseButtonReleased>())
            {
                if (mbr->button == sf::Mouse::Button::Middle)
                {
                    dragging = false;
                }
            }

            // --- Handle mouse drag movement ---
            if (event->is<sf::Event::MouseMoved>() && dragging)
            {
                sf::Vector2i newPos = sf::Mouse::getPosition(window);
                sf::Vector2f delta(
                    static_cast<float>(lastMousePos.x - newPos.x) * zoomLevel,
                    static_cast<float>(lastMousePos.y - newPos.y) * zoomLevel
                );
                camera.move(delta);
                lastMousePos = newPos;
            }
        }

        // --- Keyboard panning controls ---
        const float moveSpeed = 400.f * elapsed.asSeconds();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) camera.move(sf::Vector2f(0.f, -moveSpeed));
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) camera.move(sf::Vector2f(0.f, moveSpeed));
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) camera.move(sf::Vector2f(-moveSpeed, 0.f));
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) camera.move(sf::Vector2f(moveSpeed, 0.f));

        // --- Clamp camera to map boundaries ---
        sf::Vector2f center = camera.getCenter();
        sf::Vector2f halfSize = camera.getSize() / 2.f;
        center.x = std::clamp(center.x, halfSize.x, 2000.f - halfSize.x);
        center.y = std::clamp(center.y, halfSize.y, 2000.f - halfSize.y);
        camera.setCenter(center);

        // --- Spawn cars at gate1_junction road ---
        if (spawned_count < max_cars && spawn_timer.getElapsedTime().asSeconds() > 1.5f)
        {
            if (auto road = traffic_map.get_double_road(0)) // gate1_junction to {200.f, 465.f}
            {
                road->add_to_forward(std::make_unique<Car>(road->get_forward(), loaded ? &car_texture : nullptr));
                spawned_count++;
            }
            spawn_timer.restart();
        }

        // Update traffic map
        traffic_map.update(elapsed);

        // Apply the camera before drawing
        window.setView(camera);
        window.clear(sf::Color(20, 20, 40));
        traffic_map.draw(window); // Draw the traffic map
        window.display();
    }
}
