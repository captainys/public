set(TARGET_NAME t77dump t77analyze t77save t77load)

add_library(t77lib t77.cpp t77.h)
target_include_directories(t77lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(t77lib fm7lib)

add_executable(t77dump t77dump.cpp)
target_link_libraries(t77dump fm7lib t77lib)

add_executable(t77analyze t77analyze.cpp)
target_link_libraries(t77analyze fm7lib t77lib)

add_executable(t77save t77save.cpp)
target_link_libraries(t77save fm7lib t77lib)

add_executable(t77load t77load.cpp)
target_link_libraries(t77load fm7lib t77lib)


foreach(TARGET ${TARGET_NAME})
	set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/exe")
	set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/exe")
	set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/exe")
endforeach(TARGET)
