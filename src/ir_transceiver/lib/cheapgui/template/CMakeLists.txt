set(TARGET_NAME cheapgui_template)

add_executable(${TARGET_NAME} main.cpp)
target_link_libraries(${TARGET_NAME} cheapgui cheaplib fssimplewindow ysgl ysbitmapfont)
