set(TARGET_NAME d77analyze d77fileutil)

add_library(d77lib d77.cpp d77.h d77filesystem.cpp d77filesystem.h)
target_include_directories(d77lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(d77analyze d77analyze.cpp)
target_link_libraries(d77analyze fm7lib d77lib)

add_executable(d77fileutil d77fileutil.cpp)
target_link_libraries(d77fileutil fm7lib d77lib)



foreach(TARGET ${TARGET_NAME})
	set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/exe")
	set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/exe")
	set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/exe")
endforeach(TARGET)
