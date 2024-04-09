
set(generated_events_header ${CMAKE_CURRENT_BINARY_DIR}/events_generated.h)
add_custom_command(OUTPUT ${generated_events_header}
	COMMAND ${PYTHON_EXECUTABLE} ${LIBEVENTS_ROOT_DIR}/scripts/generate.py
		--language cpp
		--output ${CMAKE_CURRENT_BINARY_DIR}
		${LIBEVENTS_ROOT_DIR}/libs/test.json
	DEPENDS
		${LIBEVENTS_ROOT_DIR}/libs/test.json
		${LIBEVENTS_ROOT_DIR}/scripts/generate.py
		${LIBEVENTS_ROOT_DIR}/libs/cpp/templates/events_generated.h.jinja
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMENT "Generating test event header file"
)
add_custom_target(libevents_test_header DEPENDS ${generated_events_header})

add_executable(parser_test
	parser.cpp
	)
target_link_libraries(parser_test PRIVATE libevents_parser)
target_include_directories(parser_test PRIVATE ${LIBEVENTS_CPP_SOURCE_DIR})
target_include_directories(parser_test PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
add_dependencies(parser_test libevents_test_header)

