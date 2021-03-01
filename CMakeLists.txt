cmake_minimum_required(VERSION 3.17)
project(rtcpp)

option(WASM "Enable web assembly compilation" OFF)

message("WASM mode enabled: ${WASM}")

set(CMAKE_CXX_STANDARD 20)

add_compile_options(
		-D_GLIBCXX_USE_CXX11_ABI=0)

add_subdirectory(thirdparty/rapidjson)
add_subdirectory(thirdparty/libsrtp)
add_subdirectory(thirdparty/base-n)
add_subdirectory(src/rtcpplib)

if (WASM)
	add_subdirectory(src/js)
else()
	add_subdirectory(src/rtcpplibtest)
	add_subdirectory(src/java)
endif()
