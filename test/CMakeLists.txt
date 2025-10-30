find_package(Catch2 3 REQUIRED)

add_executable(${PROJECT_NAME}_tests test_main.cpp)

target_link_libraries(${PROJECT_NAME}_tests
  PRIVATE
    ${PROJECT_NAME}_lib       # link to your core logic library, not the executable
    Catch2::Catch2WithMain
)

include(Catch)
catch_discover_tests(${PROJECT_NAME}_tests)

message(STATUS "Catch2 found - tests enabled")


