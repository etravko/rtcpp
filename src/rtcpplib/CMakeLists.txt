cmake_minimum_required(VERSION 3.17)
project(librtcpp)

file(GLOB SOURCES cpp/*.cpp)

add_library(${PROJECT_NAME} ${SOURCES})

target_include_directories (${PROJECT_NAME} PUBLIC "cpp/")
target_include_directories (${PROJECT_NAME} PUBLIC "public/")

target_link_libraries(${PROJECT_NAME} PUBLIC rapidjson)
target_link_libraries(${PROJECT_NAME} PUBLIC libbasen)
target_link_libraries(${PROJECT_NAME} PUBLIC libsrtp)