cmake_minimum_required(VERSION 3.17)
project(rtcpptest)

file(GLOB SOURCES cpp/*.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PUBLIC librtcpp)
target_link_libraries(${PROJECT_NAME} PUBLIC libsrtp)

add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/data
        ${CMAKE_CURRENT_BINARY_DIR}/data)