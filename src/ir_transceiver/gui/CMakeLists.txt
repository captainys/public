set(TARGET_NAME irsensor_gui)

add_executable(${TARGET_NAME} main.cpp irdata.cpp irdata.h)
target_link_libraries(${TARGET_NAME} cheapgui cheaplib fssimplewindow fssimplefiledialog ysgl ysbitmapfont comportlib irtoylib)
