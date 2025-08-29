option(BUILD_TESTING "Build the tests for this project" ON)

if(BUILD_TESTING)
  include(CTest)
  enable_testing()
  add_subdirectory(test)
endif()
