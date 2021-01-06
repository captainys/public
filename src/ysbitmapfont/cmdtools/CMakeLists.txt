if("${CMAKE_SYSTEM_NAME}" STREQUAL "WindowsStore")
	return()
endif()

set(TARGET_NAME ysmaketextbmp)
add_executable(${TARGET_NAME} ysmaketextbmp.cpp)
target_link_libraries(${TARGET_NAME} ysclass ysbitmap ysbitmapfont)
