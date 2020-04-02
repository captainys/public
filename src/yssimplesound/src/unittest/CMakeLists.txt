set(TARGET_NAME yssimplesound_test01)
add_executable(${TARGET_NAME} main.cpp)
target_link_libraries(${TARGET_NAME} yssimplesound)

if(MSVC)
	target_link_libraries(${TARGET_NAME} yssimplesound_dsound)
elseif(APPLE)
	target_link_libraries(${TARGET_NAME} yssimplesound_macosx)
elseif(UNIX)
	target_link_libraries(${TARGET_NAME} yssimplesound_linux_alsa)
endif()



add_test(NAME yssimplesound_makeFromCDDA COMMAND ${TARGET_NAME})
