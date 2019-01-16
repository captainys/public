set(TARGET_NAME cheapgui cheapgui_gl1)



add_library(cheapgui gui.cpp gui_gl2.cpp gui.h)
add_library(cheapgui_gl1 gui.cpp gui_gl1.cpp gui.h)
target_include_directories(cheapgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(cheapgui_gl1 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(cheapgui ysgl ysbitmapfont)
target_link_libraries(cheapgui_gl1 ysgl ysbitmapfont)

