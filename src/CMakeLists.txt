# --- Library Target ---
add_library(TrafficSimulation_lib
    app_utility.cpp
    camera.cpp
    car.cpp
    bike.cpp
    truck.cpp
    double_road.cpp
    junction.cpp
    road.cpp
    traffic_light.cpp
    traffic_map.cpp
)

target_include_directories(TrafficSimulation_lib PUBLIC ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(TrafficSimulation_lib
    PRIVATE
        TrafficSimulation_options
        TrafficSimulation_warnings
        sfml-graphics
        sfml-window
        sfml-system
        sfml-audio
        sfml-network
)

# --- Executable Target ---
add_executable(TrafficSimulation main.cpp)

target_link_libraries(TrafficSimulation
    PRIVATE
        TrafficSimulation_lib
)


