set(TARGET_NAME irtoylib)

add_library(${TARGET_NAME} irtoy.cpp irtoy.h)
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${TARGET_NAME} comportlib cheaplib)
