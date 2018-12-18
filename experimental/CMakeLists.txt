if(NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "WindowsStore")
	add_subdirectory(Vulkan)
endif()
