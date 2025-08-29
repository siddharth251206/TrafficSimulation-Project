# Catch2 for unit testing
find_package(Catch2 3 QUIET)

if(Catch2_FOUND)
    add_executable(${PROJECT_NAME}_tests test_main.cpp)

    target_link_libraries(${PROJECT_NAME}_tests
      PRIVATE
        ${PROJECT_NAME}_options
        ${PROJECT_NAME}_warnings
        Catch2::Catch2WithMain
    )

    # Register tests with CTest
    include(Catch)
    catch_discover_tests(${PROJECT_NAME}_tests)

    message(STATUS "Catch2 found - tests enabled")
else()
    message(STATUS "Catch2 not found - skipping tests (see docs/vcpkg-integration.md for MinGW setup)")
endif()
