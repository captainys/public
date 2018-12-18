if(NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "WindowsStore" AND NOT ANDROID)
	add_subdirectory("batch")
	add_subdirectory("batch_graphical")
endif()
add_subdirectory("interactive")

set(YS_ALL_BATCH_TEST ${YS_ALL_BATCH_TEST} PARENT_SCOPE)
