set(TARGET_NAME disk2tape)

add_library(disk2tapelib formatdata.h formatdata.cpp)
target_include_directories(disk2tapelib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(disk2tape main.cpp formatm.cpp formatm.h uraram.cpp uraram.h dskwrite.cpp dskwrite.h tommr.cpp tommr.h)
target_link_libraries(disk2tape fm7lib disk2tapelib t77lib d77lib)



foreach(TARGET ${TARGET_NAME})
	set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/exe")
	set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/exe")
	set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/exe")
endforeach(TARGET)
