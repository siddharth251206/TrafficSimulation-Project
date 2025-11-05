#pragma once
#include "traffic_map.hpp" // Needs to be included before string
#include <SFML/System/Time.hpp> // For sf::Time
#include <string>

std::string get_executable_dir();

// --- MODIFIED ---
// Now takes the default light timings as parameters
void load_map_from_file(
    TrafficMap& traffic_map,
    const std::string& filename,
    sf::Time green_duration,
    sf::Time yellow_duration
);
// ----------------