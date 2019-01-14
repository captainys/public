set(TARGET_NAME mangoToBas)

add_executable(mangoToBas main.cpp)
target_link_libraries(mangoToBas fm7lib)



foreach(TARGET ${TARGET_NAME})
	set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/exe")
	set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/exe")
	set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/exe")
endforeach(TARGET)
