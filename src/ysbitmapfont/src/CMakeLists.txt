set(TARGET_NAME "ysbitmapfont")

if(NOT "${YS_OPENGL_VERSION}" STREQUAL "OpenGLES2")
	set(platform_SRCS ysglusefontbitmap.c)
else()
	set(platform_SRCS "")
endif()

add_library(${TARGET_NAME}
	${platform_SRCS}
	ysglfontdata.c
	ysglfontbitmaputil.c
	ysglmaketextbitmap.c
	ysglsldrawfontbitmap.c
	ysglfontdata.h
)
target_link_libraries(${TARGET_NAME} ysgl)
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
