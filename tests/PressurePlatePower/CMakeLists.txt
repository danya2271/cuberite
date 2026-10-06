add_executable(PressurePlatePower-exe PressurePlatePowerTest.cpp)
target_include_directories(PressurePlatePower-exe PRIVATE ${PROJECT_SOURCE_DIR}/src)
add_test(NAME PressurePlatePower-test COMMAND PressurePlatePower-exe)
