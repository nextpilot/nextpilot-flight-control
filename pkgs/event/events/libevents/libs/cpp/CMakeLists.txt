cmake_minimum_required(VERSION 2.8.12)

project(libevents C CXX)

set(LIBEVENTS_CPP_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(LIBEVENTS_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../..")

include_directories(include)

add_subdirectory(parse)
add_subdirectory(protocol EXCLUDE_FROM_ALL)
add_subdirectory(tests)


find_program(CLANG_FORMAT_EXECUTABLE
	NAMES clang-format clang-format-5.0
		clang-format-4.0 clang-format-3.9
		clang-format-3.8 clang-format-3.7
		clang-format-3.6 clang-format-3.5
		clang-format-3.4 clang-format-3.3
	DOC "clang-format executable"
)

file(GLOB LIBEVENTS_CPP_FORMAT_SRCS
	include/*.h
	parse/*.cpp parse/*.h
	protocol/*.cpp protocol/*.h
	tests/*.cpp tests/*.h)
if (CLANG_FORMAT_EXECUTABLE)
	message(STATUS "Enabling format target")
	add_custom_target(
        format ${CMAKE_CURRENT_SOURCE_DIR}/scripts/run_clang_format.sh
		${CLANG_FORMAT_EXECUTABLE} ${LIBEVENTS_CPP_FORMAT_SRCS}
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		)
else()
    message(STATUS "Not enabling format target (clang-format not found)")
endif()

