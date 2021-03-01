cmake_minimum_required(VERSION 3.17)
project(rtcppjs)

file(GLOB SOURCES cpp/*.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PUBLIC librtcpp)
target_compile_options(${PROJECT_NAME} PUBLIC -fexceptions)

set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "--bind -fexceptions -s MODULARIZE -s FORCE_FILESYSTEM=1 -s WASM=1 -lnodefs.js")
