add_executable(${PROJECT_NAME}_app
        main.cpp
        car.cpp
        car.h
        road.cpp
        road.h
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
