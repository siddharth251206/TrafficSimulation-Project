#include "camera.hpp"
#include "pathfinder.hpp"
#include "file_parse.hpp"
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

    // --- LOAD MAP FROM FILE ---
    // All the junction definitions and add_double_road calls
    // have been moved to "map_data.txt" and are loaded by this function.
    try
    {
        load_map_from_file(traffic_map, get_executable_dir() + "/map_data.txt");
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
    size_t max_cars = 150;

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
        if (traffic_map.get_car_count() < max_cars
            && spawn_timer.getElapsedTime().asSeconds() > 0.1f)
        {
            if (auto start_road = traffic_map.get_random_road(),
                end_road = traffic_map.get_random_road();
                start_road && end_road)
            {
                float start_distance = RNG::instance().getFloat(0.f, start_road->getLength());
                float end_distance = RNG::instance().getFloat(0.f, end_road->getLength());

                if (start_road == end_road)
                {
                    if (end_distance > start_distance)
                    {
                        auto car = std::make_unique<Car>(
                            start_road, loaded ? &car_texture : nullptr, start_distance
                        );
                        car->set_destination({}, end_road, end_distance);
                        start_road->add(std::move(car));
                    }
                }
                else if (auto start_junction = start_road->getEndJunction().lock(),
                         end_junction = end_road->getEndJunction().lock();
                         start_junction && end_junction)
                {
                    PathFinder pathfinder;
                    if (auto path = pathfinder.find_path(start_junction, end_junction);
                        !path.empty() && path.back().lock() == end_road)
                    {
                        auto car = std::make_unique<Car>(
                            start_road, loaded ? &car_texture : nullptr, start_distance
                        );
                        car->set_destination(path, end_road, end_distance);
                        start_road->add(std::move(car));
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