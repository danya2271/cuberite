add_executable(ExplosionBudget-exe ExplosionBudgetTest.cpp)
target_include_directories(ExplosionBudget-exe PRIVATE ${PROJECT_SOURCE_DIR}/src)
add_test(NAME ExplosionBudget-test COMMAND ExplosionBudget-exe)
