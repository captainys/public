#include "fsvulkan.h"




FsVulkanContext::RenderToTexture::RenderToTexture()
{
	hasColor=false;
	hasDepth=false;
}

FsVulkanContext::RenderPassBeginInfo FsVulkanContext::RenderToTexture::MakeRenderPassBeginInfo(const Frame &frame) const
{
	RenderPassBeginInfo info;

	info.beginInfo={};

	int nClearValue=0;
	if(true==hasColor)
	{
		info.clearColor[nClearValue].color.float32[0]=1.0f;
		info.clearColor[nClearValue].color.float32[1]=1.0f;
		info.clearColor[nClearValue].color.float32[2]=1.0f;
		info.clearColor[nClearValue].color.float32[3]=1.0f;
		++nClearValue;
	}
	if(true==hasDepth)
	{
		info.clearColor[nClearValue].depthStencil.depth=1.0f;
		info.clearColor[nClearValue].depthStencil.stencil=0;
		++nClearValue;
	}

	info.beginInfo.sType=VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	info.beginInfo.renderPass=this->renderPass;
	info.beginInfo.framebuffer=frame.framebuffer;
	info.beginInfo.renderArea.offset={0,0};
	info.beginInfo.renderArea.extent=frame.imageExtent;

	info.beginInfo.clearValueCount=nClearValue;
	info.beginInfo.pClearValues=info.clearColor;

	return info;
}

void FsVulkanContext::RenderToTexture::CreateShadowMap(FsVulkanContext *owner,long long int nShadowMap,long long int wid,long long int hei)
{
	this->owner=owner;

// Regular SwapChain
//			FsVulkanCreateSwapChain();
//			FsVulkanCreateRenderPass();
//			FsVulkanCreateDepthBuffer();
//			FsVulkanCreateFramebuffer();
//			FsVulkanCreateCommandBuffer();
//			FsVulkanCreateSemaphore();
	imageFormat=VK_FORMAT_D32_SFLOAT;  // If it doesn't support it, just get another GPU.

	CreateShadowMapRenderPass(wid,hei);
	CreateShadowMapDepthBuffer(nShadowMap,wid,hei);
	CreateFramebuffer();
	CreateUniformPool();
	CreateShadowMapSampler(wid,hei);
	CreateCommandBuffer();

	hasColor=false;
	hasDepth=true;

	VkSemaphoreCreateInfo semaphorInfo={};
	semaphorInfo.sType=VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	vkCreateSemaphore(owner->logicalDevice,&semaphorInfo,nullptr,&renderFinishedSemaphore);
}

void FsVulkanContext::RenderToTexture::CreateShadowMapRenderPass(long long int wid,long long int hei)
{
	VkAttachmentDescription depthAttachment={};
	depthAttachment.format=imageFormat;
	depthAttachment.samples=VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp=VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp=VK_ATTACHMENT_STORE_OP_STORE;
	depthAttachment.stencilLoadOp=VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp=VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout=VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout=VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;

	VkAttachmentReference depthAttachmentRef={};
	depthAttachmentRef.attachment=0;   // <- This works as an index.
	depthAttachmentRef.layout=VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass={};
	subpass.pipelineBindPoint=VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.pDepthStencilAttachment=&depthAttachmentRef;

	VkSubpassDependency dependency[2]={{},{}};
	dependency[0].srcSubpass=VK_SUBPASS_EXTERNAL;
	dependency[0].dstSubpass=0;
	dependency[0].srcStageMask=VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	dependency[0].srcAccessMask=0;
	dependency[0].dstStageMask=VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	dependency[0].dstAccessMask=VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT|VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	dependency[0].dependencyFlags=VK_DEPENDENCY_BY_REGION_BIT;

	dependency[1].srcSubpass=0;
	dependency[1].dstSubpass=VK_SUBPASS_EXTERNAL;
	dependency[1].srcStageMask=VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	dependency[1].srcAccessMask=VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT|VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	dependency[1].dstStageMask=VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	dependency[1].dstAccessMask=VK_ACCESS_SHADER_READ_BIT;
	dependency[1].dependencyFlags=VK_DEPENDENCY_BY_REGION_BIT;
	// I still don't fully understand the logic though.

	VkAttachmentDescription attachment[]={depthAttachment};

	VkRenderPassCreateInfo renderPassInfo={};
	renderPassInfo.sType=VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount=1;
	renderPassInfo.pAttachments=attachment;
	renderPassInfo.subpassCount=1;
	renderPassInfo.pSubpasses=&subpass;
	renderPassInfo.dependencyCount=sizeof(dependency)/sizeof(dependency[0]);
	renderPassInfo.pDependencies=dependency;
	if(VK_SUCCESS!=vkCreateRenderPass(owner->logicalDevice,&renderPassInfo,nullptr,&renderPass))
	{
		printf("Failed to create a render pass.\n");
		printf("Doomsday.\n");
	}
	else
	{
		printf("Created render pass!\n");
	}
}

void FsVulkanContext::RenderToTexture::CreateShadowMapDepthBuffer(long long int nShadowMap,long long int wid,long long int hei)
{
	VkImageCreateInfo imageInfo={};
	imageInfo.sType=VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType=VK_IMAGE_TYPE_2D;
	imageInfo.extent.width=wid;
	imageInfo.extent.height=hei;
	imageInfo.extent.depth=1;
	imageInfo.mipLevels=1;
	imageInfo.arrayLayers=1; // ?
	imageInfo.format=imageFormat;
	imageInfo.tiling=VK_IMAGE_TILING_OPTIMAL;
	// imageInfo.initialLayout=VK_IMAGE_LAYOUT_UNDEFINED; // Do I need this?
	imageInfo.usage=VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT|VK_IMAGE_USAGE_SAMPLED_BIT;
	imageInfo.sharingMode=VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples=VK_SAMPLE_COUNT_1_BIT;

	frame.resize(nShadowMap);
	for(long long int i=0; i<frame.size(); ++i)
	{
		auto &f=frame[i];
		f.owner=this;
		f.imageExtent.width=wid;
		f.imageExtent.height=hei;
		f.imageIndex=i;

		if(VK_SUCCESS!=vkCreateImage(owner->logicalDevice,&imageInfo,nullptr,&f.depthImage))
		{
			printf("Cannot create image.\n");
			return;
		}

		f.depthMemory=owner->AllocMemoryForImage(f.depthImage,VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		if(VK_NULL_HANDLE==f.depthMemory)
		{
			vkDestroyImage(owner->logicalDevice,f.depthImage,nullptr);
			f.depthImage=VK_NULL_HANDLE;
			return;
		}
		vkBindImageMemory(owner->logicalDevice,f.depthImage,f.depthMemory,0);

		VkImageViewCreateInfo imageViewInfo={};
		imageViewInfo.sType=VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewInfo.image=f.depthImage;
		imageViewInfo.viewType=VK_IMAGE_VIEW_TYPE_2D;
		imageViewInfo.format=imageFormat;
		imageViewInfo.subresourceRange.aspectMask=VK_IMAGE_ASPECT_DEPTH_BIT;  // <- Difference from image view for swap chain.
		imageViewInfo.subresourceRange.baseMipLevel=0;
		imageViewInfo.subresourceRange.levelCount=1;
		imageViewInfo.subresourceRange.baseArrayLayer=0;
		imageViewInfo.subresourceRange.layerCount=1;
		if(VK_SUCCESS!=vkCreateImageView(owner->logicalDevice,&imageViewInfo,nullptr,&f.depthImageView))
		{
			printf("Failed to create an image view.\n");
			vkFreeMemory(owner->logicalDevice,f.depthMemory,0);
			vkDestroyImage(owner->logicalDevice,f.depthImage,nullptr);
			f.depthImage=VK_NULL_HANDLE;
			f.depthMemory=VK_NULL_HANDLE;
		}
	}
}

void FsVulkanContext::RenderToTexture::CreateShadowMapSampler(long long int wid,long long int hei)
{
	VkSamplerCreateInfo samplerInfo={};
	samplerInfo.sType=VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter=VK_FILTER_LINEAR;
	samplerInfo.minFilter=VK_FILTER_LINEAR;
	samplerInfo.addressModeU=VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerInfo.addressModeV=VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerInfo.addressModeW=VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	// samplerInfo.anisotropyEnable=VK_TRUE;
	samplerInfo.maxAnisotropy=1; // I have a feeling that anisotropy should be off.
	samplerInfo.borderColor=VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates=VK_FALSE;
	samplerInfo.compareEnable=VK_FALSE;
	samplerInfo.compareOp=VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode=VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.minLod=0;
	samplerInfo.maxLod=1;
	if(VK_SUCCESS!=vkCreateSampler(owner->logicalDevice,&samplerInfo,nullptr,&sampler))
	{
		printf("Failed to create a sampler.\n");
		printf("I'm tired.\n");
	}
}

void FsVulkanContext::RenderToTexture::DestroyRenderToTexture(void)
{
	vkDestroySemaphore(owner->logicalDevice,renderFinishedSemaphore,nullptr);
	renderFinishedSemaphore=VK_NULL_HANDLE;
	vkDestroySampler(owner->logicalDevice,sampler,nullptr);
	sampler=VK_NULL_HANDLE;
	DestroyRenderTarget();
}

FsVulkanContext::Texture2d FsVulkanContext::RenderToTexture::GetDepthTexture2d(long long int idx) const
{
	Texture2d tex;
	tex.image=VK_NULL_HANDLE;
	tex.imageView=frame[idx].depthImageView;
	tex.sampler=sampler;
	tex.mem=VK_NULL_HANDLE;
	return tex;
}
