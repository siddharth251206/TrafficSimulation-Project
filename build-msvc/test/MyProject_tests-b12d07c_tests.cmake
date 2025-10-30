add_test( Test C:/Users/aniru/Documents/TrafficSimulation-Project/build-msvc/test/RelWithDebInfo/MyProject_tests.exe Test  )
set_tests_properties( Test PROPERTIES WORKING_DIRECTORY C:/Users/aniru/Documents/TrafficSimulation-Project/build-msvc/test SKIP_RETURN_CODE 4)
set( MyProject_tests_TESTS Test)
