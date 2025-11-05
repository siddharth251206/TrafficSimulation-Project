add_executable(${PROJECT_NAME}_app
        main.cpp
        car.cpp
        road.cpp
	      app_utility.cpp
        junction.cpp
		    traffic_map.cpp
        double_road.cpp
        traffic_light.cpp
        camera.cpp
        pathfinder.cpp
        file_parse.cpp
)

target_link_libraries(${PROJECT_NAME}_app
  PRIVATE
    ${PROJECT_NAME}_options
    ${PROJECT_NAME}_warnings

    # Link library targets here, for example:
    # fmt::fmt
    SFML::System
    SFML::Window
    SFML::Graphics
    SFML::Audio
)

# --- NEW COMMAND ADDED ---
# This command automatically copies your map data file from the source directory
# (src) to the binary/build directory (build/.../src) where the executable is.
# This ensures the executable can always find it.
add_custom_command(
    TARGET ${PROJECT_NAME}_app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_CURRENT_SOURCE_DIR}/map_data.txt"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}_app>"
    COMMENT "Copying map_data.txt to build directory"
)