set(TARGET_NAME d77Server)

add_executable(${TARGET_NAME} d77server.cpp clientbin.h clientbin.cpp diskimg.h diskimg.cpp)
target_link_libraries(${TARGET_NAME} comportlib disk2tapelib d77lib fm7lib)

foreach(TARGET ${TARGET_NAME})
	set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/exe")
	set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/exe")
	set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/exe")
endforeach(TARGET)
