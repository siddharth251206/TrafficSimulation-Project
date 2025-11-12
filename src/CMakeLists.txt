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
        ux.cpp
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

# This command automatically copies your map data file from the source directory
# (src) to the binary/build directory (build/.../src) where the executable is.
set(DATA_FILE_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/map_data.txt")
set(DATA_FILE_DEST "${CMAKE_CURRENT_BINARY_DIR}/map_data.txt")

# Create a custom target that copies the file
add_custom_command(
    OUTPUT ${DATA_FILE_DEST}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${DATA_FILE_SOURCE} ${DATA_FILE_DEST}
    DEPENDS ${DATA_FILE_SOURCE}
    COMMENT "Copying map_data.txt..."
)

add_custom_target(copy_map_data
    DEPENDS ${DATA_FILE_DEST}
)
# Make your main app DEPEND on this target
add_dependencies(${PROJECT_NAME}_app copy_map_data)
