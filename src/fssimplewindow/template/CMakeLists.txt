if("${CMAKE_SYSTEM_NAME}" STREQUAL "WindowsStore")
	# This template is not available in UWP and iOS
	return()
endif()

set(TARGET_NAME fssimplewindow_template)
add_executable(${TARGET_NAME} MACOSX_BUNDLE main.cpp)
target_link_libraries(${TARGET_NAME} fssimplewindow)
