cmake_minimum_required(VERSION 3.16)
project(TrafficSimulationProject LANGUAGES CXX)

# ✅ Use modern LLVM compiler
set(CMAKE_CXX_COMPILER "/opt/homebrew/opt/llvm/bin/clang++")
set(CMAKE_C_COMPILER "/opt/homebrew/opt/llvm/bin/clang")

# ✅ Use C++23 (needed for std::views::values)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(${PROJECT_NAME}_app
        main.cpp
        car.cpp
        road.cpp
	      app_utility.cpp
        junction.cpp
		    traffic_map.cpp
        double_road.cpp
        traffic_light.cpp
)

target_link_libraries(${PROJECT_NAME}_app
  PRIVATE
    SFML::System
    SFML::Window
    SFML::Graphics
    SFML::Audio
)

