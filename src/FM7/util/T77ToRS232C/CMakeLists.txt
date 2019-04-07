set(TARGET_NAME t77Server)

add_executable(${TARGET_NAME} main.cpp bioshook_small.cpp bioshook_small.h bioshook_buffered.cpp bioshook_buffered.h ../rs232cLoader/strloader.h ../rs232cLoader/strloader.cpp)
target_link_libraries(${TARGET_NAME} t77lib comportlib)

foreach(TARGET ${TARGET_NAME})
	set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/exe")
	set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/exe")
	set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/exe")
	target_include_directories(${TARGET} PRIVATE ../rs232cLoader)
endforeach(TARGET)
