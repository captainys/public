#include <vector>
#include <string>

#include "fssimplewindow.h"
#include "fsvulkan.h"



FsVulkanContext::RendererPool::RendererPool(FsVulkanContext *owner)
{
	this->owner=owner;
}

void FsVulkanContext::RendererPool::DestroyRenderer(void)
{
	for(auto rendererPtr : createdRenderer)
	{
		rendererPtr->CleanUp();
	}
	createdRenderer.clear();
}

