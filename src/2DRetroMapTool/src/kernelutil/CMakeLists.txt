set(TARGET_NAME retromap_kernelutil)
add_library(${TARGET_NAME} 
	retromap_screenshotcache.cpp
	retromap_pngexport.cpp
	retromap_screenshotcache.h
	retromap_pngexport.h
)
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${TARGET_NAME} retromap_kernel ysport)
