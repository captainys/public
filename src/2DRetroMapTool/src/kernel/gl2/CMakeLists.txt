set(TARGET_NAME retromap_kernel_gl2)
add_library(${TARGET_NAME} retromap_data_render.cpp)
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${TARGET_NAME} ysclass ystexturemanager ystexturemanager_gl ysglcpp ysglcpp_gl2)
