set(TARGET_NAME retromap_kernel)
add_library(${TARGET_NAME}
	retromap_data.cpp
	retromap_data_io.cpp
	retromap_type.h
	retromap_kernel.h
	retromap_data.h
)
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${TARGET_NAME} ysclass ysbitmaputil ystexturemanager ystexturemanager_gl yssystemfont ysglcpp)
