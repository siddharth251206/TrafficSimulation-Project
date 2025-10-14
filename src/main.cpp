// #include "traffic_light.hpp"
// #include "traffic_map.hpp"
// #include <SFML/Graphics.hpp>
// #include <iostream>
// #include <memory>
// #include <optional>
// #include <string>

// // Constants for map boundaries and zoom limits
// constexpr float MAP_MIN_X = 0.f;
// constexpr float MAP_MAX_X = 2000.f;
// constexpr float MAP_MIN_Y = 0.f;
// constexpr float MAP_MAX_Y = 2000.f;
// constexpr float MIN_ZOOM = 0.5f;  // Prevent zooming in too far
// constexpr float MAX_ZOOM = 2.0f;  // Prevent zooming out too far
// constexpr float ZOOM_FACTOR = 0.1f; // Smoother zoom increment
// constexpr float PAN_SPEED = 800.f; // Increased for smoother panning

// int main()
// {
//     constexpr unsigned int width = 1200;
//     constexpr unsigned int height = 800;
//     sf::RenderWindow window{ sf::VideoMode{ { width, height } }, "Traffic Simulator" };
//     window.setFramerateLimit(60);

//     // Initialize traffic map
//     TrafficMap traffic_map;

//     // ------ SVNIT MAP ------
//     const sf::Vector2f gajjar_junction = { 700.f, 100.f };
//     const sf::Vector2f bhabha_junction = { 200.f, 100.f };
//     const sf::Vector2f nehru_junction = { 100.f, 100.f };
//     const sf::Vector2f swami_junction = { 100.f, 200.f };
//     const sf::Vector2f swami_entrance_junction = { 200.f, 200.f };
//     const sf::Vector2f quarters_junction = { 700.f, 375.f };
//     const sf::Vector2f canteen_junction = { 200.f, 375.f };
//     const sf::Vector2f temple_junction = { 900.f, 300.f };
//     const sf::Vector2f gate1_junction = { 200.f, 700.f };
//     const sf::Vector2f gate2_junction = { 700.f, 700.f };
//     const sf::Vector2f library_junction = { 300.f, 375.f };
//     const sf::Vector2f admin_junction = { 300.f, 500.f };
//     const sf::Vector2f department_junction = { 700.f, 500.f };

//     // Add roads to the traffic map
//     traffic_map.add_double_road(gate1_junction, { 200.f, 465.f }, 15.f, false);
//     traffic_map.add_double_road(bhabha_junction, gajjar_junction, 15.f);
//     traffic_map.add_double_road(nehru_junction, bhabha_junction, 15.f);
//     traffic_map.add_double_road(nehru_junction, swami_junction, 15.f);
//     traffic_map.add_double_road(bhabha_junction, swami_entrance_junction, 15.f);
//     traffic_map.add_double_road(swami_junction, swami_entrance_junction, 15.f);
//     traffic_map.add_double_road(swami_entrance_junction, { 200.f, 300.f }, 15.f);
//     traffic_map.add_double_road({ 200.f, 300.f }, { 700.f, 300.f }, 15.f);
//     traffic_map.add_double_road(gajjar_junction, { 700.f, 300.f }, 15.f);
//     traffic_map.add_double_road({ 200.f, 300.f }, canteen_junction, 15.f);
//     traffic_map.add_double_road(canteen_junction, quarters_junction, 15.f);
//     traffic_map.add_double_road({ 700.f, 300.f }, quarters_junction, 15.f);
//     traffic_map.add_double_road({ 700.f, 300.f }, temple_junction, 15.f);
//     traffic_map.add_double_road(quarters_junction, { 900.f, 375.f }, 15.f);
//     traffic_map.add_double_road(gajjar_junction, { 900.f, 100.f }, 15.f);
//     traffic_map.add_double_road({ 900.f, 100.f }, temple_junction, 15.f);
//     traffic_map.add_double_road({ 900.f, 375.f }, temple_junction, 15.f);
//     traffic_map.add_double_road({ 100.f, 375.f }, canteen_junction, 15.f);
//     traffic_map.add_double_road(canteen_junction, { 200.f, 465.f }, 15.f);
//     traffic_map.add_double_road({ 100.f, 465.f }, { 200.f, 465.f }, 15.f);
//     traffic_map.add_double_road({ 100.f, 465.f }, { 100.f, 375.f }, 15.f);
//     traffic_map.add_double_road({ 100.f, 465.f }, { 100.f, 700.f }, 15.f);
//     traffic_map.add_double_road({ 100.f, 700.f }, gate1_junction, 15.f);
//     traffic_map.add_double_road(gate1_junction, gate2_junction, 15.f);
//     traffic_map.add_double_road(library_junction, admin_junction, 15.f);
//     traffic_map.add_double_road(admin_junction, { 300.f, 700.f }, 15.f);
//     traffic_map.add_double_road(department_junction, gate2_junction, 15.f);
//     traffic_map.add_double_road(department_junction, quarters_junction, 15.f);
//     traffic_map.add_double_road(department_junction, admin_junction, 15.f);

//     // Load car texture
//     sf::Texture car_texture;
//     bool loaded = false;
//     loaded = AssetHelper::try_load_texture(car_texture, "assets/premium_car.png", "premium car");
//     if (!loaded)
//         loaded = AssetHelper::try_load_texture(car_texture, "assets/advanced_car.png", "advanced car");
//     if (!loaded)
//         loaded = AssetHelper::try_load_texture(car_texture, "assets/car.png", "simple car");

//     // Car spawner logic
//     sf::Clock spawn_timer;
//     int max_cars = 20;
//     int spawned_count = 0;

//     // Initialize camera
//     sf::View camera(sf::FloatRect(sf::Vector2f{0.f, 0.f}, sf::Vector2f{static_cast<float>(width), static_cast<float>(height)}));
//     camera.setCenter(sf::Vector2f(600.f, 400.f));
//     float zoomLevel = 1.f;
//     bool dragging = false;
//     sf::Vector2i lastMousePos;

//     sf::Clock clock;
//     while (window.isOpen())
//     {
//         sf::Time elapsed = clock.restart();
//         float deltaTime = elapsed.asSeconds();

//         // Handle events
//         while (const std::optional<sf::Event> event = window.pollEvent())
//         {
//             if (event->is<sf::Event::Closed>())
//                 window.close();

//             // Smooth zooming centered on mouse cursor
//             if (const auto* wheel = event->getIf<sf::Event::MouseWheelScrolled>())
//             {
//                 sf::Vector2i mousePos = sf::Mouse::getPosition(window);
//                 sf::Vector2f worldPos = window.mapPixelToCoords(mousePos, camera);

//                 float oldZoom = zoomLevel;
//                 if (wheel->delta > 0)
//                     zoomLevel = std::max(MIN_ZOOM, zoomLevel * (1.f - ZOOM_FACTOR)); // Zoom in
//                 else
//                     zoomLevel = std::min(MAX_ZOOM, zoomLevel * (1.f + ZOOM_FACTOR)); // Zoom out

//                 camera.setSize(sf::Vector2f(width * zoomLevel, height * zoomLevel));

//                 // Adjust camera to keep the mouse's world position fixed
//                 sf::Vector2f newWorldPos = window.mapPixelToCoords(mousePos, camera);
//                 sf::Vector2f offset = worldPos - newWorldPos;
//                 camera.move(offset);
//             }

//             // Handle mouse dragging
//             if (const auto* mbp = event->getIf<sf::Event::MouseButtonPressed>())
//             {
//                 if (mbp->button == sf::Mouse::Button::Middle)
//                 {
//                     dragging = true;
//                     lastMousePos = sf::Mouse::getPosition(window);
//                 }
//             }
//             else if (const auto* mbr = event->getIf<sf::Event::MouseButtonReleased>())
//             {
//                 if (mbr->button == sf::Mouse::Button::Middle)
//                     dragging = false;
//             }

//             if (event->is<sf::Event::MouseMoved>() && dragging)
//             {
//                 sf::Vector2i newPos = sf::Mouse::getPosition(window);
//                 sf::Vector2f delta(
//                     static_cast<float>(lastMousePos.x - newPos.x) * zoomLevel * 0.5f, // Reduced sensitivity
//                     static_cast<float>(lastMousePos.y - newPos.y) * zoomLevel * 0.5f
//                 );
//                 camera.move(delta);
//                 lastMousePos = newPos;
//             }
//         }

//         // Smooth keyboard panning
//         sf::Vector2f panDelta(0.f, 0.f);
//         if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) panDelta.y -= PAN_SPEED * deltaTime;
//         if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) panDelta.y += PAN_SPEED * deltaTime;
//         if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) panDelta.x -= PAN_SPEED * deltaTime;
//         if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) panDelta.x += PAN_SPEED * deltaTime;

//         // Apply easing to panning for smoother movement
//         camera.move(panDelta * 0.9f); // Easing factor for smooth movement

//         // Clamp camera to map boundaries
//         sf::Vector2f center = camera.getCenter();
//         sf::Vector2f halfSize = camera.getSize() / 2.f;
//         center.x = std::clamp(center.x, MAP_MIN_X + halfSize.x, MAP_MAX_X - halfSize.x);
//         center.y = std::clamp(center.y, MAP_MIN_Y + halfSize.y, MAP_MAX_Y - halfSize.y);
//         camera.setCenter(center);

//         // Spawn cars
//         if (spawned_count < max_cars && spawn_timer.getElapsedTime().asSeconds() > 1.5f)
//         {
//             if (auto road = traffic_map.get_double_road(0))
//             {
//                 road->add_to_forward(std::make_unique<Car>(road->get_forward(), loaded ? &car_texture : nullptr));
//                 spawned_count++;
//             }
//             spawn_timer.restart();
//         }

//         // Update and render
//         traffic_map.update(elapsed);
//         window.setView(camera);
//         window.clear(sf::Color(20, 20, 40));
//         traffic_map.draw(window);
//         window.display();
//     }
// }

#include "traffic_light.hpp"
#include "traffic_map.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <memory>
#include <optional>
#include <string>

// Constants for map boundaries and zoom limits
constexpr float MAP_MIN_X = 0.f;
constexpr float MAP_MAX_X = 2000.f;
constexpr float MAP_MIN_Y = 0.f;
constexpr float MAP_MAX_Y = 2000.f;
constexpr float MIN_ZOOM = 0.5f;  // Prevent zooming in too far
constexpr float ZOOM_FACTOR = 0.1f; // Smooth zoom increment
constexpr float PAN_SPEED = 800.f; // Smooth panning speed

int main()
{
    constexpr unsigned int width = 1200;
    constexpr unsigned int height = 800;
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

    // Load car texture
    sf::Texture car_texture;
    bool loaded = false;
    loaded = AssetHelper::try_load_texture(car_texture, "assets/premium_car.png", "premium car");
    if (!loaded)
        loaded = AssetHelper::try_load_texture(car_texture, "assets/advanced_car.png", "advanced car");
    if (!loaded)
        loaded = AssetHelper::try_load_texture(car_texture, "assets/car.png", "simple car");

    // Car spawner logic
    sf::Clock spawn_timer;
    int max_cars = 20;
    int spawned_count = 0;

    // Initialize camera
    sf::View camera(sf::FloatRect(sf::Vector2f{0.f, 0.f}, sf::Vector2f{static_cast<float>(width), static_cast<float>(height)}));
    camera.setCenter(sf::Vector2f(600.f, 400.f));
    float zoomLevel = 1.f;
    bool dragging = false;
    sf::Vector2i lastMousePos;

    // Calculate maximum zoom level based on map size
    const float MAX_ZOOM = std::max((MAP_MAX_X - MAP_MIN_X) / width, (MAP_MAX_Y - MAP_MIN_Y) / height);

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

            // Smooth zooming centered on mouse cursor
            if (const auto* wheel = event->getIf<sf::Event::MouseWheelScrolled>())
            {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                sf::Vector2f worldPos = window.mapPixelToCoords(mousePos, camera);

                float oldZoom = zoomLevel;
                if (wheel->delta > 0)
                    zoomLevel = std::max(MIN_ZOOM, zoomLevel * (1.f - ZOOM_FACTOR)); // Zoom in
                else
                    zoomLevel = std::min(MAX_ZOOM, zoomLevel * (1.f + ZOOM_FACTOR)); // Zoom out

                // Safety check for valid camera size
                if (zoomLevel > 0.f)
                {
                    camera.setSize(sf::Vector2f(width * zoomLevel, height * zoomLevel));

                    // Adjust camera to keep the mouse's world position fixed
                    sf::Vector2f newWorldPos = window.mapPixelToCoords(mousePos, camera);
                    sf::Vector2f offset = worldPos - newWorldPos;
                    camera.move(offset);
                }
                else
                {
                    // Reset to default zoom if invalid
                    zoomLevel = 1.f;
                    camera.setSize(sf::Vector2f(width, height));
                    camera.setCenter(sf::Vector2f(600.f, 400.f));
                }
            }

            // Handle mouse dragging
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
                    dragging = false;
            }

            if (event->is<sf::Event::MouseMoved>() && dragging)
            {
                sf::Vector2i newPos = sf::Mouse::getPosition(window);
                sf::Vector2f delta(
                    static_cast<float>(lastMousePos.x - newPos.x) * zoomLevel * 0.5f,
                    static_cast<float>(lastMousePos.y - newPos.y) * zoomLevel * 0.5f
                );
                camera.move(delta);
                lastMousePos = newPos;
            }
        }

        // Smooth keyboard panning
        sf::Vector2f panDelta(0.f, 0.f);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) panDelta.y -= PAN_SPEED * deltaTime;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) panDelta.y += PAN_SPEED * deltaTime;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) panDelta.x -= PAN_SPEED * deltaTime;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) panDelta.x += PAN_SPEED * deltaTime;

        // Apply easing to panning
        camera.move(panDelta * 0.9f);

        // Clamp camera to map boundaries with safety checks
        sf::Vector2f center = camera.getCenter();
        sf::Vector2f halfSize = camera.getSize() / 2.f;

        // Ensure clamping bounds are valid
        float minX = MAP_MIN_X + halfSize.x;
        float maxX = MAP_MAX_X - halfSize.x;
        float minY = MAP_MIN_Y + halfSize.y;
        float maxY = MAP_MAX_Y - halfSize.y;

        // Prevent invalid clamp ranges
        if (minX <= maxX)
            center.x = std::clamp(center.x, minX, maxX);
        else
            center.x = (MAP_MIN_X + MAP_MAX_X) / 2.f; // Center if invalid

        if (minY <= maxY)
            center.y = std::clamp(center.y, minY, maxY);
        else
            center.y = (MAP_MIN_Y + MAP_MAX_Y) / 2.f; // Center if invalid

        camera.setCenter(center);

        // Spawn cars
        if (spawned_count < max_cars && spawn_timer.getElapsedTime().asSeconds() > 1.5f)
        {
            if (auto road = traffic_map.get_double_road(0))
            {
                road->add_to_forward(std::make_unique<Car>(road->get_forward(), loaded ? &car_texture : nullptr));
                spawned_count++;
            }
            spawn_timer.restart();
        }

        // Update and render
        traffic_map.update(elapsed);
        window.setView(camera);
        window.clear(sf::Color(20, 20, 40));
        traffic_map.draw(window);
        window.display();
    }
}