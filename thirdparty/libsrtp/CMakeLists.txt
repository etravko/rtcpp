cmake_minimum_required(VERSION 3.17)
project(libsrtp)

add_library(${PROJECT_NAME} INTERFACE)

target_include_directories (${PROJECT_NAME} INTERFACE include/)

if (WASM)

	message("Use WASM configuration for srtp lib")
	target_link_libraries (${PROJECT_NAME}
	        INTERFACE
	            "${PROJECT_SOURCE_DIR}/bin/wasm/libsrtp2.a")
	
else()

	message("Use native configuration for srtp lib")
	target_link_libraries (${PROJECT_NAME}
	        INTERFACE
	            "$<$<CONFIG:DEBUG>:${PROJECT_SOURCE_DIR}/bin/debug/srtp2.lib>"
	            "$<$<CONFIG:RELEASE>:${PROJECT_SOURCE_DIR}/bin/release/srtp2.lib>")

endif()

