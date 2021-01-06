set(TARGET_NAME ysbitmaputil)
add_library(${TARGET_NAME}
	ysbitmapdrawing.cpp
	ysbitmapcontour.cpp
	ysbitmaputil.h
	ysbitmapcontour.h
	ysbitmapdrawing.h
)
target_link_libraries(${TARGET_NAME} ysbitmap ysclass)
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
