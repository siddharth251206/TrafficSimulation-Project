cmake_minimum_required(VERSION 3.25)

# --- vcpkg Integration ---
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    include(cmake/vcpkg-root-finder.cmake)
    find_vcpkg_root()
    if(VCPKG_ROOT_FOUND)
        set(CMAKE_TOOLCHAIN_FILE "${VCPKG_TOOLCHAIN_PATH}")
        message(STATUS "Using vcpkg toolchain (fallback): ${CMAKE_TOOLCHAIN_FILE}")
    else()
        message(WARNING "vcpkg not found. Install vcpkg and set VCPKG_ROOT environment variable, or use CMake presets.")
    endif()
endif()

# --- Project Info ---
project(
  TrafficSimulation
  VERSION 0.1.0
  DESCRIPTION "A Traffic Simulation Project using SFML"
  LANGUAGES CXX
)

# --- Configurations ---
include(cmake/project-options.cmake)
include(cmake/dependencies.cmake)

# --- SFML Dependency ---
find_package(SFML CONFIG REQUIRED COMPONENTS Graphics Window System Audio Network)

# --- Option and Warning Targets ---
add_library(${PROJECT_NAME}_options INTERFACE)
add_library(${PROJECT_NAME}_warnings INTERFACE)

if(COMMAND set_project_warnings)
    set_project_warnings(${PROJECT_NAME}_warnings)
endif()

if(COMMAND set_project_options)
    set_project_options(${PROJECT_NAME}_options)
endif()

# --- Add Library (Core Logic) ---
add_library(${PROJECT_NAME}_lib
    src/car.cpp
    src/bike.cpp
    src/truck.cpp
    src/road.cpp
    src/traffic_light.cpp
    src/junction.cpp
    src/double_road.cpp
    src/camera.cpp
    src/app_utility.cpp
    src/traffic_map.cpp
)

target_include_directories(${PROJECT_NAME}_lib
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME}_lib
    PRIVATE
        ${PROJECT_NAME}_options
        ${PROJECT_NAME}_warnings
        SFML::Graphics
        SFML::Window
        SFML::System
        SFML::Audio
        SFML::Network
)

# --- Main Executable ---
add_executable(${PROJECT_NAME} src/main.cpp)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        ${PROJECT_NAME}_lib
        ${PROJECT_NAME}_options
        ${PROJECT_NAME}_warnings
        SFML::Graphics
        SFML::Window
        SFML::System
        SFML::Audio
        SFML::Network
)

# --- Testing (only for root project) ---
if(PROJECT_IS_TOP_LEVEL)
    include(cmake/testing.cmake)
endif()