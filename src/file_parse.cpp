#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <filesystem>

#include "traffic_map.hpp"
#include "file_parse.hpp" 

#if defined(_WIN32)
#include <windows.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#elif defined(__linux__)
#include <limits.h>
#include <unistd.h>
#endif

// ... get_executable_dir implementation (keep as is) ...
std::string get_executable_dir()
{
    // ... (Keep your existing implementation) ...
    char buffer[4096];
    #if defined(_WIN32)
        DWORD len = GetModuleFileNameA(NULL, buffer, sizeof(buffer));
        if (len > 0) return std::filesystem::path(std::string(buffer, len)).parent_path().string();
    #endif
    return "."; // Fallback
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
        line.erase(0, line.find_first_not_of(" \t\n\r"));
        line.erase(line.find_last_not_of(" \t\n\r") + 1);

        if (line.empty() || line[0] == '#') continue;

        std::stringstream ss(line);
        std::string line_type;
        ss >> line_type;

        if (line_type == "junction")
        {
            std::string name;
            float x, y;
            if (ss >> name >> x >> y)
                junctions[name] = { x, y };
        }
        else if (line_type == "road")
        {
            std::string j1_name, j2_name, divider_str;
            float speed_limit; // Changed variable name from 'width' to 'speed_limit'

            // The new map format is: road <Start> <End> <Speed> <Divider>
            if (ss >> j1_name >> j2_name >> speed_limit >> divider_str)
            {
                bool need_divider = (divider_str == "true");

                if (junctions.find(j1_name) == junctions.end() || junctions.find(j2_name) == junctions.end())
                {
                    std::cerr << "Error: Unknown junction in road def at line " << line_number << std::endl;
                    continue;
                }

                // FIXED: We pass 'speed_limit' to traffic_map.
                // We provide a hardcoded '22.f' for visual width, since the file doesn't provide it anymore.
                traffic_map.add_double_road(
                    junctions[j1_name], 
                    junctions[j2_name], 
                    speed_limit,    // Max Speed
                    22.f,           // Visual Width (Standardized)
                    need_divider
                );
            }
        }
        else if (line_type == "lights")
        {
            std::string junction_name;
            if (ss >> junction_name)
            {
                if (junctions.count(junction_name))
                    traffic_map.get_junction(junctions[junction_name])->install_lights();
            }
        }
    }
    std::cout << "Successfully loaded map data from: " << filename << std::endl;
}