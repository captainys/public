#include "fsvulkan.h"


static const unsigned char allOne2dSrc[4*4*4]=
{
	255,255,255,255, 255,255,255,255, 255,255,255,255, 255,255,255,255,
	255,255,255,255, 255,255,255,255, 255,255,255,255, 255,255,255,255,
	255,255,255,255, 255,255,255,255, 255,255,255,255, 255,255,255,255,
	255,255,255,255, 255,255,255,255, 255,255,255,255, 255,255,255,255,
};
static const unsigned char allZero2dSrc[4*4*4]=
{
	  0,  0,  0,  0,   0,  0,  0,  0,   0,  0,  0,  0,   0,  0,  0,  0,
	  0,  0,  0,  0,   0,  0,  0,  0,   0,  0,  0,  0,   0,  0,  0,  0,
	  0,  0,  0,  0,   0,  0,  0,  0,   0,  0,  0,  0,   0,  0,  0,  0,
	  0,  0,  0,  0,   0,  0,  0,  0,   0,  0,  0,  0,   0,  0,  0,  0,
};
static const unsigned char checker2dSrc[8*8*4]=
{
	  0,  0,  0,255, 255,255,255,255,   0,  0,  0,255, 255,255,255,255,   0,  0,  0,255, 255,255,255,255,   0,  0,  0,255, 255,255,255,255,
	255,255,255,255,   0,  0,  0,255, 255,255,255,255,   0,  0,  0,255, 255,255,255,255,   0,  0,  0,255, 255,255,255,255,   0,  0,  0,255, 
	  0,  0,  0,255, 255,255,255,255,   0,  0,  0,255, 255,255,255,255,   0,  0,  0,255, 255,255,255,255,   0,  0,  0,255, 255,255,255,255,
	255,255,255,255,   0,  0,  0,255, 255,255,255,255,   0,  0,  0,255, 255,255,255,255,   0,  0,  0,255, 255,255,255,255,   0,  0,  0,255, 
	  0,  0,  0,255, 255,255,255,255,   0,  0,  0,255, 255,255,255,255,   0,  0,  0,255, 255,255,255,255,   0,  0,  0,255, 255,255,255,255,
	255,255,255,255,   0,  0,  0,255, 255,255,255,255,   0,  0,  0,255, 255,255,255,255,   0,  0,  0,255, 255,255,255,255,   0,  0,  0,255, 
	  0,  0,  0,255, 255,255,255,255,   0,  0,  0,255, 255,255,255,255,   0,  0,  0,255, 255,255,255,255,   0,  0,  0,255, 255,255,255,255,
	255,255,255,255,   0,  0,  0,255, 255,255,255,255,   0,  0,  0,255, 255,255,255,255,   0,  0,  0,255, 255,255,255,255,   0,  0,  0,255, 
};


void FsVulkanContext::StockResource::Create(FsVulkanContext &vulkan)
{
	allOne2d=vulkan.CreateTexture2d(4,4,allOne2dSrc);
	allZero2d=vulkan.CreateTexture2d(4,4,allZero2dSrc);
	checker2d=vulkan.CreateTexture2d(8,8,checker2dSrc);
}
void FsVulkanContext::StockResource::CleanUp(FsVulkanContext &vulkan)
{
	vulkan.DestroyTexture2d(allOne2d);
	vulkan.DestroyTexture2d(allZero2d);
	vulkan.DestroyTexture2d(checker2d);
}

FsVulkanContext::Texture2d FsVulkanContext::GetStockAllOneTexture2d(void) const
{
	return stock.allOne2d;
}
FsVulkanContext::Texture2d FsVulkanContext::GetStockAllZeroTexture2d(void) const
{
	return stock.allZero2d;
}
FsVulkanContext::Texture2d FsVulkanContext::GetStockCheckerTexture2d(void) const
{
	return stock.checker2d;
}
