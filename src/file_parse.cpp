#include <fstream>// For file reading
#include <iostream>
#include <map>// To store junction names and coordinates
#include <sstream>// For string parsing
#include <stdexcept>// For error handling

#include "traffic_map.hpp"

#if defined(_WIN32)
#include <windows.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#elif defined(__linux__)
#include <limits.h>
#include <unistd.h>
#endif

std::string get_executable_dir()
{
    char buffer[4096];
    std::string exePath;

#if defined(_WIN32)
    DWORD len = GetModuleFileNameA(NULL, buffer, sizeof(buffer));
    if (len == 0 || len == sizeof(buffer))
        throw std::runtime_error("Failed to get executable path");
    exePath.assign(buffer, len);

#elif defined(__APPLE__)
    uint32_t size = sizeof(buffer);
    if (_NSGetExecutablePath(buffer, &size) != 0)
        throw std::runtime_error("Buffer too small; increase buffer size");
    exePath = buffer;

#elif defined(__linux__)
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (len == -1)
        throw std::runtime_error("Failed to get executable path");
    buffer[len] = '\0';
    exePath = buffer;

#else
#error "Unsupported platform"
#endif

    // Use C++17 filesystem to get the parent directory
    std::filesystem::path pathObj(exePath);
    return pathObj.parent_path().string();
}

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
                // std::cout << "Loaded junction: " << name << " at (" << x << ", " << y << ")" <<
                // std::endl;
            }
            else
            {
                std::cerr << "Error parsing junction at line " << line_number << ": " << line
                          << std::endl;
            }
        }
        else if (line_type == "road")
        {
            std::string j1_name, j2_name, divider_str;
            float width;
            bool need_divider = true;// Default from your original code

            if (ss >> j1_name >> j2_name >> width >> divider_str)
            {
                need_divider = (divider_str == "true");

                if (junctions.find(j1_name) == junctions.end())
                {
                    std::cerr << "Error: Unknown junction '" << j1_name << "' at line "
                              << line_number << std::endl;
                    continue;
                }
                if (junctions.find(j2_name) == junctions.end())
                {
                    std::cerr << "Error: Unknown junction '" << j2_name << "' at line "
                              << line_number << std::endl;
                    continue;
                }

                traffic_map.add_double_road(
                    junctions[j1_name], junctions[j2_name], width, need_divider
                );
                // std::cout << "Loaded road: " << j1_name << " to " << j2_name << std::endl;
            }
            else
            {
                std::cerr << "Error parsing road at line " << line_number << ": " << line
                          << std::endl;
            }
        }
        else if (line_type == "lights")
        {
            std::string junction_name;
            if (ss >> junction_name)
            {
                if (junctions.find(junction_name) == junctions.end())
                {
                    std::cerr << "Error: Unknown junction to install light: '" << junction_name
                              << "' at line " << line_number << std::endl;
                    continue;
                }
                traffic_map.get_junction(junctions[junction_name])->install_light(sf::seconds(5));
            }
            else
            {
                std::cerr << "Error parsing light installation at line " << line_number << ": "
                          << line << std::endl;
            }
        }
    }

    std::cout << "Successfully loaded map data from: " << filename << std::endl;
}