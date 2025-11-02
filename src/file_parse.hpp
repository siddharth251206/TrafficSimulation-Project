#pragma once
#include <string>
#include "traffic_map.hpp"

std::string getExecutableDir();
void load_map_from_file(TrafficMap& traffic_map, const std::string& filename);
