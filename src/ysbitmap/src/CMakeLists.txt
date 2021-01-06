set(TARGET_NAME "ysbitmap")

add_library(${TARGET_NAME}
	ysbitmap.cpp
	ysbitmaparray.cpp
	ysbitmappngif.cpp
	yspng.cpp
	yspngenc.cpp
	ysbitmap.h
	ysbitmaparray.h
	yspng.h
	yspngenc.h
	ysbitmaptemplate.h
)
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
