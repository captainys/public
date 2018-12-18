set(TARGET_NAME Vulkan01-FsOpenWindow)
add_executable(${TARGET_NAME} MACOSX_BUNDLE main.cpp)
target_link_libraries(${TARGET_NAME} fsvulkanwindow)


set(VULKAN_SDK $ENV{VULKAN_SDK})
set(VULKAN_DLL_DESTINATION "$<TARGET_FILE_DIR:${TARGET_NAME}>")
set(VULKAN_DLL_SOURCE)
if(APPLE)
	set(VULKAN_DLL_SOURCE ${VULKAN_SDK}/lib/libMoltenVK.dylib)
	# set(VULKAN_DLL_SOURCE /Users/soji/Vulkan/MoltenVK/MoltenVK/macOS/libMoltenVK.dylib)
endif()

if(VULKAN_DLL_SOURCE)
	add_custom_command(TARGET ${TARGET_NAME} POST_BUILD 
		COMMAND echo For:  ${TARGET_NAME}
		COMMAND echo Copy
		COMMAND echo From: ${VULKAN_DLL_SOURCE}
		COMMAND echo To:   ${VULKAN_DLL_DESTINATION}
		COMMAND "${CMAKE_COMMAND}" -E copy \"${VULKAN_DLL_SOURCE}\" \"${VULKAN_DLL_DESTINATION}\"
    )
endif()
