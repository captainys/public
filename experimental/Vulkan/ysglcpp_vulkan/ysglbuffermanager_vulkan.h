#ifndef YSGLBUFFERMANAGER_VULKAN_IS_INCLUDED
#define YSGLBUFFERMANAGER_VULKAN_IS_INCLUDED
/* { */

#include <ysclass.h>
#include <ysglcpp.h>
#include <ysglbuffermanager.h>

#include "fsvulkan.h"


class YsGLBufferManager::ActualBuffer
{
friend class YsGLBufferManager;
friend class YsGLBufferManager::Unit;

private:
	enum
	{
		max_num_bind_buffer=8  // Should never go above 4. (vtxBuf,nomBuf,colBuf,pointSizeBuf.)  Maybe 5 when texCoordBuf is added.
	};
	FsVulkanContext::Buffer vtxBuf,nomBuf,colBuf,pointSizeBuf;
public:
	int nVtx;

	ActualBuffer();
	~ActualBuffer();
	void CleanUp(void);
	void DrawPrimitiveVtx(VkCommandBuffer cmdBuf,const FsVulkanContext::Renderer *renderer) const;
	void DrawPrimitiveVtxNom(VkCommandBuffer cmdBuf,const FsVulkanContext::Renderer *renderer) const;
	void DrawPrimitiveVtxCol(VkCommandBuffer cmdBuf,const FsVulkanContext::Renderer *renderer) const;
	void DrawPrimitiveVtxNomCol(VkCommandBuffer cmdBuf,const FsVulkanContext::Renderer *renderer) const;
	void DrawPrimitiveVtxColPointSize(VkCommandBuffer cmdBuf,const FsVulkanContext::Renderer *renderer) const;
};

/* } */
#endif
