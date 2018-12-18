#include "MoltenVK/mvk_vulkan.h"
#include "MoltenVK/vk_mvk_moltenvk.h"
#include "vulkan/vulkan.h"

#include "fsvulkan.h"



const char *FsVulkanGetPlatformSurfacExtensionName(void)
{
#ifdef VK_USE_PLATFORM_IOS_MVK
	return VK_MVK_IOS_SURFACE_EXTENSION_NAME
#elif defined(VK_USE_PLATFORM_MACOS_MVK)
	return VK_MVK_MACOS_SURFACE_EXTENSION_NAME;
#endif
}

