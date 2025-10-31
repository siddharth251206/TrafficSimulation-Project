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
