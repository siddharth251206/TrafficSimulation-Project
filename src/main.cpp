#include "camera.hpp"
#include "traffic_light.hpp"
#include "traffic_map.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <memory>
#include <optional>
#include <string>

// --- NEW INCLUDES ---
#include <fstream>   // For file reading
#include <sstream>   // For string parsing
#include <map>       // To store junction names and coordinates
#include <stdexcept> // For error handling
#include <filesystem>


/**
 * @brief Loads map data from a text file and populates the TrafficMap.
 *
 * This function reads a specified file, parsing junction definitions and
 * road connections to build the map.
 *
 * File Format:
 * # Comments are ignored
 * junction <name> <x_coord> <y_coord>
 * road <junction_name_1> <junction_name_2> <width> <need_divider_bool (true/false)>
 *
 * @param traffic_map The TrafficMap object to populate.
 * @param filename The path to the map data file.
 */

std::string get_map_path() {
    std::filesystem::path exe_dir = std::filesystem::current_path();
        return (exe_dir / "../../../src/map_data.txt").string();
}
void load_map_from_file(TrafficMap& traffic_map, const std::string& filename)
{
    std::map<std::string, sf::Vector2f> junctions;
    std::ifstream file(filename);

    if (!file.is_open())
    {
        std::cerr << "Error: Could not open map file: " << filename << std::endl;
        throw std::runtime_error("Could not open map file: " + filename);
    }

    std::string line;
    int line_number = 0;

    while (std::getline(file, line))
    {
        line_number++;
        // Trim whitespace (simple version)
        line.erase(0, line.find_first_not_of(" \t\n\r"));
        line.erase(line.find_last_not_of(" \t\n\r") + 1);

        // Skip empty lines or comments
        if (line.empty() || line[0] == '#')
        {
            continue;
        }

        std::stringstream ss(line);
        std::string line_type;
        ss >> line_type;

        if (line_type == "junction")
        {
            std::string name;
            float x, y;
            if (ss >> name >> x >> y)
            {
                junctions[name] = { x, y };
                // std::cout << "Loaded junction: " << name << " at (" << x << ", " << y << ")" << std::endl;
            }
            else
            {
                std::cerr << "Error parsing junction at line " << line_number << ": " << line << std::endl;
            }
        }
        else if (line_type == "road")
        {
            std::string j1_name, j2_name, divider_str;
            float width;
            bool need_divider = true; // Default from your original code

            if (ss >> j1_name >> j2_name >> width >> divider_str)
            {
                need_divider = (divider_str == "true");

                if (junctions.find(j1_name) == junctions.end())
                {
                    std::cerr << "Error: Unknown junction '" << j1_name << "' at line " << line_number << std::endl;
                    continue;
                }
                if (junctions.find(j2_name) == junctions.end())
                {
                    std::cerr << "Error: Unknown junction '" << j2_name << "' at line " << line_number << std::endl;
                    continue;
                }

                traffic_map.add_double_road(junctions[j1_name], junctions[j2_name], width, need_divider);
                // std::cout << "Loaded road: " << j1_name << " to " << j2_name << std::endl;
            }
            else
            {
                std::cerr << "Error parsing road at line " << line_number << ": " << line << std::endl;
            }
        }
    }

    std::cout << "Successfully loaded map data from: " << filename << std::endl;
}

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
        load_map_from_file(traffic_map, get_map_path());
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

        traffic_map.update(elapsed);
        window.setView(camera_controller.get_camera());
        window.clear(sf::Color(20, 20, 40));
        traffic_map.draw(window);
        window.display();
    }
}
