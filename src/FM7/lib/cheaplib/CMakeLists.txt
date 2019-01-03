set(TARGET_NAME cheaplib)

add_library(${TARGET_NAME} cheaputil.cpp cheaputil.h)
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
