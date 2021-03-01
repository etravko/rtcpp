cmake_minimum_required(VERSION 3.17)

find_package(Java 11 REQUIRED)
find_package(Java COMPONENTS Development)
find_package(JNI REQUIRED)

include(UseJava)

# java source build

file(GLOB_RECURSE JAVA_SOURCES
	*.java)

add_jar(rtcppjava
    ${JAVA_SOURCES}
    GENERATE_NATIVE_HEADERS rtcppjava-native
)

# cpp source build

file(GLOB_RECURSE CPP_SOURCES
		cpp/*.cpp)

add_library(rtcppjni SHARED
		${CPP_SOURCES})

target_include_directories(rtcppjni PRIVATE
    ${JNI_INCLUDE_DIRS})

target_link_libraries(rtcppjni PRIVATE
	rtcppjava-native)

target_link_libraries(rtcppjni PUBLIC 
	librtcpp)

# reassemble jar with native binary

message(STATUS "bin dir = ${Java_JAR_EXECUTABLE}")

set(TMP_JAR_DIR rtcppjava.tmp)
add_custom_target(rtcppjni_package
		DEPENDS rtcppjava rtcppjni
		BYPRODUCTS rtcp.jar
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		COMMAND ${CMAKE_COMMAND} -E remove_directory ${TMP_JAR_DIR}
		COMMAND ${CMAKE_COMMAND} -E make_directory ${TMP_JAR_DIR}
		COMMAND ${CMAKE_COMMAND} -E make_directory ${TMP_JAR_DIR}/libs
		COMMAND ${CMAKE_COMMAND} -E copy rtcppjni.dll ${TMP_JAR_DIR}/libs/
		COMMAND ${CMAKE_COMMAND} -E copy rtcppjava.jar rtcpp.jar
		COMMAND ${CMAKE_COMMAND} -E chdir ${TMP_JAR_DIR} ${Java_JAR_EXECUTABLE} -v --update --file ../rtcpp.jar libs
		COMMENT "Jar file: ${CMAKE_CURRENT_BINARY_DIR}/rtcpp.jar")

