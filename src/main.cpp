#include "camera.hpp"
#include "pathfinder.hpp"
#include "file_parse.hpp"
#include "traffic_map.hpp"
#include "ux.hpp"
#include "app_utility.hpp"

#include <SFML/Graphics.hpp>
#include <iostream>
#include <memory>
#include <optional>
#include <string>

int main()
{
    unsigned int width = 1300;
    unsigned int height = 800;

    // --- 🖥️ Get desktop resolution dynamically (SFML 3 style) ---
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::Vector2u screenSize = desktop.size;

    // --- Create resizable window (90 % of screen) ---
    sf::Vector2u windowSize(
        static_cast<unsigned int>(screenSize.x * 0.9f),
        static_cast<unsigned int>(screenSize.y * 0.9f)
    );

    sf::RenderWindow window(
        sf::VideoMode(windowSize),
        "Traffic Simulation - Interactive UI",
        sf::Style::Close | sf::Style::Resize
    );

    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);

    // --- Initialize traffic map ---
    TrafficMap traffic_map;

    // --- Load map from file ---
    try
    {
        load_map_from_file(traffic_map, get_executable_dir() + "/map_data.txt");
        std::cout << "[Info] Map loaded successfully.\n";
    }
    catch (const std::exception& e)
    {
        std::cerr << "[Error] Failed to load map: " << e.what() << std::endl;
        return -1;
    }

    // --- Vehicle textures ---
    sf::Texture car_texture;
    bool loaded = false;
    loaded = AssetHelper::try_load_texture(car_texture, "assets/premium_car.png", "premium car");
    if (!loaded)
        loaded = AssetHelper::try_load_texture(car_texture, "assets/advanced_car.png", "advanced car");
    if (!loaded)
        loaded = AssetHelper::try_load_texture(car_texture, "assets/car.png", "simple car");

    sf::Clock spawn_timer;
    size_t max_cars = 150;

    // --- Camera controller ---
    CameraController camera_controller(static_cast<float>(windowSize.x), static_cast<float>(windowSize.y));
    

    // --- 🆕 Font setup (graceful fallback) ---
    sf::Font font;
    if (!font.openFromFile(get_executable_dir() + "/../../assets/contemporary-regular.ttf"))
    {
        std::cerr << "[Warning] Default system font not found — using empty font.\n";
    }

    UXController ux(window, camera_controller, traffic_map, &car_texture, &font);

    // --- Main loop ---
    sf::Clock clock;
    while (window.isOpen())
    {
        sf::Time elapsed = clock.restart();
        float deltaTime = elapsed.asSeconds();

        // --- Event handling ---
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
            ux.handle_event(*event);
        }

        // --- Update camera & UI ---
        camera_controller.handle_kb_panning(deltaTime);
        camera_controller.clamp_camera(window);

        ux.update(deltaTime);
        ux.spawn_cars();

        // --- Auto-spawn random traffic ---
        if (traffic_map.get_car_count() < max_cars
            && spawn_timer.getElapsedTime().asSeconds() > 0.2f)
        {
            if (auto start_road = traffic_map.get_random_road(),
                end_road = traffic_map.get_random_road();
                start_road && end_road)
            {
                float start_distance = RNG::instance().getFloat(0.f, start_road->getLength());
                float end_distance   = RNG::instance().getFloat(0.f, end_road->getLength());

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
                         end_junction   = end_road->getEndJunction().lock();
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
        // --- Draw everything ---
        traffic_map.update(elapsed);

        // window.setView(camera_controller.get_camera());
        // window.clear(sf::Color(20, 20, 40));
        // traffic_map.draw(window);
        // ux.render_ui();
        // window.display();

        window.clear(sf::Color(20, 20, 40));

        // --- Draw world/map using camera ---
        window.setView(camera_controller.get_camera());
        traffic_map.draw(window);

        // --- Draw UI using default view ---
        window.setView(window.getDefaultView());
        ux.render_ui();

        window.display();

    }
    return 0;
}