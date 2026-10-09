add_executable(Minecarts-exe
	MinecartSerializerTest.cpp
	${PROJECT_SOURCE_DIR}/src/WorldStorage/MinecartSerializer.cpp
	${PROJECT_SOURCE_DIR}/src/WorldStorage/FastNBT.cpp
	${PROJECT_SOURCE_DIR}/src/StringUtils.cpp
)
target_include_directories(Minecarts-exe PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(Minecarts-exe fmt::fmt)
if (WIN32)
	target_link_libraries(Minecarts-exe ws2_32)
endif()
add_test(NAME Minecarts-test COMMAND Minecarts-exe)
