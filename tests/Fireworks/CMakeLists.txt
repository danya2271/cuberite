add_executable(Fireworks-exe
	FireworksTest.cpp
	${PROJECT_SOURCE_DIR}/src/WorldStorage/FireworksSerializer.cpp
	${PROJECT_SOURCE_DIR}/src/WorldStorage/FastNBT.cpp
	${PROJECT_SOURCE_DIR}/src/StringUtils.cpp
)
target_include_directories(Fireworks-exe PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(Fireworks-exe fmt::fmt)
if (WIN32)
	target_link_libraries(Fireworks-exe ws2_32)
endif()
add_test(NAME Fireworks-test COMMAND Fireworks-exe)
