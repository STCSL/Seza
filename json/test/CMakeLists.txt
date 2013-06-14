include_directories(${GTEST_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/../include)

add_executable(testJson testJsonSerializer.cpp)
target_link_libraries(testJson ${GTEST_BOTH_LIBRARIES})

add_test(testSerializers testJson)