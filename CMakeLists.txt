cmake_minimum_required(VERSION 3.25)

# --- vcpkg Integration Fallback ---
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

# TODO: Change 'MyProject' to your actual project name
project(
  MyProject
  VERSION 0.1.0
  DESCRIPTION "A modern C++23 project"
  LANGUAGES CXX
)

# --- Configuration (C++ standards, build types, etc.)---
include(cmake/project-options.cmake)

# --- Dependencies ---
include(cmake/dependencies.cmake)

# --- Project Targets (includes directory, interface targets for options and warnings) ---
include(cmake/project-targets.cmake)

# --- Add Source Code ---
add_subdirectory(src)

# --- Testing (if this is the main project) ---
if(PROJECT_IS_TOP_LEVEL)
  include(cmake/testing.cmake)
endif()
