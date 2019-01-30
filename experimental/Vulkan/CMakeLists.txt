if(VULKAN_SDK)

add_subdirectory(fsvulkanwindow/src)
add_subdirectory(fsvulkanwindow/experiments)
add_subdirectory(fsvulkanwindow/samples/01-FsOpenWindow)
add_subdirectory(fsvulkanwindow/samples/02-Plain2dRenderer)
add_subdirectory(fsvulkanwindow/samples/03-Texture2dRenderer)
add_subdirectory(fsvulkanwindow/samples/04-Plain3dRenderer)
add_subdirectory(fsvulkanwindow/samples/05-Shaded3dRenderer)
add_subdirectory(fsvulkanwindow/samples/06-ShadowMap)
add_subdirectory(fsvulkanwindow/samples/07-PointSprite)
add_subdirectory(ysglcpp_vulkan)

endif()
