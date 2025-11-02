#include "camera.hpp"
#include "traffic_light.hpp"
#include "traffic_map.hpp"
#include "pathfinder.hpp"
#include "file_parse.hpp"
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

    // --- LOAD MAP FROM FILE ---
    // All the junction definitions and add_double_road calls
    // have been moved to "map_data.txt" and are loaded by this function.
    try
    {
        load_map_from_file(traffic_map, getExecutableDir() + "/map_data.txt");
        // load_map_from_file(traffic_map, "map_data.txt");
    }
    catch (const std::exception& e)
    {
        std::cerr << "Failed to load map: " << e.what() << std::endl;
        return -1; // Exit if map loading fails
    }
    // -----------------------------------------------------------------------

    // ----------- VEHICLES & CAMERA -------------
    sf::Texture car_texture;
    bool loaded = false;
    loaded = AssetHelper::try_load_texture(car_texture, "assets/premium_car.png", "premium car");
    if (!loaded)
        loaded =
            AssetHelper::try_load_texture(car_texture, "assets/advanced_car.png", "advanced car");
    if (!loaded)
        loaded = AssetHelper::try_load_texture(car_texture, "assets/car.png", "simple car");

    sf::Clock spawn_timer;
    int max_cars = 25;// larger map → more cars
    int spawned_count = 0;

    CameraController camera_controller(static_cast<float>(width), static_cast<float>(height));

    sf::Clock clock;
    while (window.isOpen())
    {
        sf::Time elapsed = clock.restart();
        float deltaTime = elapsed.asSeconds();

        while (const std::optional<sf::Event> event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (const auto* wrz = event->getIf<sf::Event::Resized>())
            {
                width = wrz->size.x;
                height = wrz->size.y;
            }

            if (const auto* wheel = event->getIf<sf::Event::MouseWheelScrolled>())
                camera_controller.handle_zoom(window, wheel, width, height);

            camera_controller.handle_mouse_drag(window, event);
        }

        camera_controller.handle_kb_panning(deltaTime);
        camera_controller.clamp_camera();

        // Spawn cars
        if (spawned_count < max_cars && spawn_timer.getElapsedTime().asSeconds() > 0.5f)
        {
            auto start_junction = traffic_map.get_junction({200, 700});
            auto end_junction = traffic_map.get_junction({700, 100}); // e.g., Department junction

            PathFinder pathfinder;
            if (start_junction && end_junction)
            {
                // 4. Calculate the optimal path using the Pathfinder
                std::deque<std::weak_ptr<Road>> path = pathfinder.find_path(start_junction, end_junction);

                // 5. Only spawn the car if a valid path was found
                if (!path.empty())
                {
                    // Get the very first road segment from the calculated path
                    if (auto first_road = path.front().lock())
                    {
                        // Create the car and assign it the path
                        auto car = std::make_unique<Car>(first_road, loaded ? &car_texture : nullptr);
                        car->set_path(path);

                        // Add the car to the first road
                        first_road->add(std::move(car));

                        spawned_count++;
                    }
                }
            }
            spawn_timer.restart();
        }

        traffic_map.update(elapsed);
        window.setView(camera_controller.get_camera());
        window.clear(sf::Color(20, 20, 40));
        traffic_map.draw(window);
        window.display();
    }
}
