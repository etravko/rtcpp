cmake_minimum_required(VERSION 3.17)
project(rapidjson)

add_library(${PROJECT_NAME} INTERFACE)

target_include_directories (${PROJECT_NAME} INTERFACE ".")
