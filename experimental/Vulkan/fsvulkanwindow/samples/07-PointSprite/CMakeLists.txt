set(TARGET_NAME Vulkan07-PointSprite)
add_executable(${TARGET_NAME} MACOSX_BUNDLE main.cpp)
target_link_libraries(${TARGET_NAME} fsvulkanwindow ysgl)


set(VULKAN_SDK $ENV{VULKAN_SDK})
set(VULKAN_DLL_DESTINATION "$<TARGET_FILE_DIR:${TARGET_NAME}>")
set(VULKAN_DLL_SOURCE)
if(APPLE)
	set(VULKAN_DLL_SOURCE ${VULKAN_SDK}/lib/libMoltenVK.dylib ${VULKAN_SDK}/lib/libVkLayer_core_validation.dylib)
	# set(VULKAN_DLL_SOURCE /Users/soji/Vulkan/MoltenVK/MoltenVK/macOS/libMoltenVK.dylib)
endif()

if(VULKAN_DLL_SOURCE)
	foreach(SRC ${VULKAN_DLL_SOURCE})
		add_custom_command(TARGET ${TARGET_NAME} POST_BUILD 
			COMMAND echo For:  ${TARGET_NAME}
			COMMAND echo Copy
			COMMAND echo From: ${SRC}
			COMMAND echo To:   ${VULKAN_DLL_DESTINATION}
			COMMAND "${CMAKE_COMMAND}" -E copy \"${SRC}\" \"${VULKAN_DLL_DESTINATION}\"
	    )
	endforeach(SRC)
endif()
