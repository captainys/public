#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>
#include "fssimplewindow.h"
#include "fsvulkan.h"

// $env:PATH+=";C:\VulkanSDK\1.1.73.0\bin"
// macosx/fsmacosxvulkan.cpp, 01-CreateInstance/*

const char *FsVulkanGetPlatformSurfacExtensionName(void)
{
	return VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
}

