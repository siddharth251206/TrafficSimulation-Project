# Interface libraries for configs and warnings
add_library(${PROJECT_NAME}_options INTERFACE)
add_library(${PROJECT_NAME}_warnings INTERFACE)

# Set compiler warnings 
include(cmake/project-warnings.cmake)

# Apply C++ standard to options target
target_compile_features(${PROJECT_NAME}_options INTERFACE cxx_std_${CMAKE_CXX_STANDARD})

# Target aliases
add_library(${PROJECT_NAME}::options ALIAS ${PROJECT_NAME}_options)
add_library(${PROJECT_NAME}::warnings ALIAS ${PROJECT_NAME}_warnings)

# Include directories
target_include_directories(${PROJECT_NAME}_options INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
