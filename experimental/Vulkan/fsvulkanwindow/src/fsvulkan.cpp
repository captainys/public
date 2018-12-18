#include <vector>
#include <string>

#include "fssimplewindow.h"
#include "fsvulkan.h"


// References:
//    https://vulkan-tutorial.com/Drawing_a_triangle/Presentation/Swap_chain (2018/06/17)
//    cube.c in Vulkan SDK 1.1.73.0 (2018/06/17)
//    cube.c in MoltenVK downloaded (2018/06/16)

////////////////////////////////////////////////////////////

VKAPI_ATTR VkBool32 VKAPI_CALL FsVulkanDebugMessageCallBack(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
	void *pUserData)
{
	printf("Vulkan: %s\n",pCallbackData->pMessage);
	return false; // true to crash, false to go on.
}

////////////////////////////////////////////////////////////

FsVulkanContext::OneTimeCommandGuard::OneTimeCommandGuard(const FsVulkanContext &context)
{
	contextPtr=&context;

 	VkCommandBufferAllocateInfo allocInfo={};
 	allocInfo.sType=VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
 	allocInfo.level=VK_COMMAND_BUFFER_LEVEL_PRIMARY;
 	allocInfo.commandPool=context.commandPool;
 	allocInfo.commandBufferCount=1;

	vkAllocateCommandBuffers(context.logicalDevice,&allocInfo,&cmdBuf);

	VkCommandBufferBeginInfo beginInfo={};
	beginInfo.sType=VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags=VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(cmdBuf,&beginInfo);

}
FsVulkanContext::OneTimeCommandGuard::~OneTimeCommandGuard()
{
	vkEndCommandBuffer(cmdBuf);

	VkSubmitInfo info={};
	info.sType=VK_STRUCTURE_TYPE_SUBMIT_INFO;
	info.commandBufferCount=1;
	info.pCommandBuffers=&cmdBuf;

	// vkQueueWaitIdle ?
	vkQueueSubmit(contextPtr->graphicsQueue,1,&info,VK_NULL_HANDLE);
	vkQueueWaitIdle(contextPtr->graphicsQueue);

	vkFreeCommandBuffers(contextPtr->logicalDevice,contextPtr->commandPool,1,&cmdBuf);
}

////////////////////////////////////////////////////////////

FsVulkanContext::Texture2d::Texture2d()
{
	image=VK_NULL_HANDLE;
	imageView=VK_NULL_HANDLE;
	sampler=VK_NULL_HANDLE;
	mem=VK_NULL_HANDLE;
}

void FsVulkanContext::Texture2d::CleanUp(VkDevice logicalDevice)
{
	vkDestroyImageView(logicalDevice,imageView,nullptr);
	vkDestroySampler(logicalDevice,sampler,nullptr);
	vkDestroyImage(logicalDevice,image,nullptr);
	vkFreeMemory(logicalDevice,mem,nullptr);

	image=VK_NULL_HANDLE;
	imageView=VK_NULL_HANDLE;
	sampler=VK_NULL_HANDLE;
	mem=VK_NULL_HANDLE;
}

////////////////////////////////////////////////////////////

FsVulkanContext::Renderer::UniformBinding FsVulkanContext::Renderer::GetUniformBinding(int idx) const
{
	return uniformBinding[idx];
}

const std::vector <FsVulkanContext::Renderer::VTATTRIBTYPE> &FsVulkanContext::Renderer::GetAttribBinding(void) const
{
	return attribBinding;
}

////////////////////////////////////////////////////////////

void FsVulkanContext::SwapChain::CreateSwapChain(void)
{
	auto surfaceFom=FsVulkanSelectSurfaceFormat(FsVulkanEnumeratePhysicalDeviceSurfaceFormat(owner->physicalDevice,owner->surface));
	auto presentMode=FsVulkanSelectPresentMode(FsVulkanEnumeratePhysicalDeviceSurfacePresentationModesKHR(owner->physicalDevice,owner->surface));

	VkSurfaceCapabilitiesKHR surfaceCap;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(owner->physicalDevice,owner->surface,&surfaceCap);
	auto extent=FsVulkanSelectSwapExtent(surfaceCap);

	// Tutorial says the following two will be needed in the future.
	this->imageFormat=surfaceFom.format;

	printf("Width=%d, Height=%d\n",extent.width,extent.height);
	printf("minImageCount=%d\n",surfaceCap.minImageCount);

	// I'm happy with double buffer.  I don't bother having an additional buffer like the tutorial.
	VkSwapchainCreateInfoKHR info={};
	info.sType=VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	info.pNext=nullptr;
	info.flags=0;
	info.surface=owner->surface;
	info.minImageCount=surfaceCap.minImageCount;
	info.imageFormat=surfaceFom.format;
	info.imageColorSpace=surfaceFom.colorSpace;
	info.imageExtent=extent;
	info.imageArrayLayers=1; // 2 for stereo, according to the tutorial.
	info.imageUsage=VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	uint32_t graphicsQueueIndex=0;
	uint32_t presentQueueIndex=0;
	FsVulkanGetQueueFamilyIndex(graphicsQueueIndex,presentQueueIndex,owner->physicalDevice,owner->surface);
	const uint32_t queueIndexArray[2]={graphicsQueueIndex,presentQueueIndex};
	if(graphicsQueueIndex!=presentQueueIndex) // As suggested by the tutorial, but can I just go with EXCLUSIVE all the time?
	{
		info.imageSharingMode=VK_SHARING_MODE_CONCURRENT;
		info.queueFamilyIndexCount=2;
		info.pQueueFamilyIndices=queueIndexArray;
	}
	else
	{
		info.imageSharingMode=VK_SHARING_MODE_EXCLUSIVE;
		info.queueFamilyIndexCount=0;
		info.pQueueFamilyIndices=nullptr;
	}

	info.preTransform=surfaceCap.currentTransform;
	info.compositeAlpha=VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	info.presentMode=presentMode;
	info.clipped=VK_TRUE;
	info.oldSwapchain=VK_NULL_HANDLE;

	if(VK_SUCCESS==vkCreateSwapchainKHR(owner->logicalDevice,&info,nullptr,&swapChain))
	{
		printf("Created a swap chain!\n");
		uint32_t nImage;
		std::vector <VkImage> swapChainImage;
		vkGetSwapchainImagesKHR(owner->logicalDevice,swapChain,&nImage,nullptr);
		swapChainImage.resize(nImage);
		vkGetSwapchainImagesKHR(owner->logicalDevice,swapChain,&nImage,swapChainImage.data());
		printf("%d images.\n",nImage);

		frame.resize(nImage);

		for(long long int i=0; i<nImage; ++i)
		{
			frame[i].owner=this;
			frame[i].imageExtent=extent;
			frame[i].imageIndex=(uint32_t)i;
			frame[i].image=swapChainImage[i];

			VkImageViewCreateInfo info={};
			info.sType=VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			info.image=swapChainImage[i];
			info.viewType=VK_IMAGE_VIEW_TYPE_2D;
			info.format=surfaceFom.format;
			info.components.r=VK_COMPONENT_SWIZZLE_IDENTITY;
			info.components.g=VK_COMPONENT_SWIZZLE_IDENTITY;
			info.components.b=VK_COMPONENT_SWIZZLE_IDENTITY;
			info.components.a=VK_COMPONENT_SWIZZLE_IDENTITY;
			info.subresourceRange.aspectMask=VK_IMAGE_ASPECT_COLOR_BIT;
			info.subresourceRange.baseMipLevel=0;
			info.subresourceRange.levelCount=1;
			info.subresourceRange.baseArrayLayer=0;
			info.subresourceRange.layerCount=1;
			if(VK_SUCCESS!=vkCreateImageView(owner->logicalDevice,&info,nullptr,&frame[i].imageView))
			{
				printf("Failed to create an image view.\n");
				printf("Perhaps the end of the world.\n");
			}
		}
	}
	else
	{
		printf("Failed to create a swap chain.\n");
		printf("Must really be the end of the world.\n");
		exit(1);
	}
}

void FsVulkanContext::SwapChain::DestroySwapChain(void)
{
	vkDestroySwapchainKHR(owner->logicalDevice,swapChain,nullptr);
}

FsVulkanContext::Frame &FsVulkanContext::SwapChain::ReadyFrame(void)
{
	vkQueueWaitIdle(owner->presentQueue);

	uint32_t idx;
	vkAcquireNextImageKHR(owner->logicalDevice,swapChain,std::numeric_limits<uint64_t>::max(),owner->imageAvailableSemaphore,VK_NULL_HANDLE,&idx);

	// Nothing should consume per-frame resources at this point.  Should be able to de-allocate now.
	frame[idx].DestroyAllPerFrameResource();

	return frame[idx];
}

FsVulkanContext::RenderPassBeginInfo FsVulkanContext::SwapChain::MakeRenderPassBeginInfo(const Frame &frame) const
{
	RenderPassBeginInfo info;

	info.beginInfo={};

	info.clearColor[0].color.float32[0]=1.0f;
	info.clearColor[0].color.float32[1]=1.0f;
	info.clearColor[0].color.float32[2]=1.0f;
	info.clearColor[0].color.float32[3]=1.0f;

	info.clearColor[1].depthStencil.depth=1.0f;
	info.clearColor[1].depthStencil.stencil=0;

	info.beginInfo.sType=VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	info.beginInfo.renderPass=this->renderPass;
	info.beginInfo.framebuffer=frame.framebuffer;
	info.beginInfo.renderArea.offset={0,0};
	info.beginInfo.renderArea.extent=frame.imageExtent;

	info.beginInfo.clearValueCount=2;
	info.beginInfo.pClearValues=info.clearColor;

	return info;
}


////////////////////////////////////////////////////////////

void FsVulkanContext::RenderTarget::CreateRenderPass(void)
{
	VkAttachmentDescription colorAttachment={};
	colorAttachment.format=imageFormat;
	colorAttachment.samples=VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp=VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp=VK_ATTACHMENT_STORE_OP_STORE;

	colorAttachment.stencilLoadOp=VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp=VK_ATTACHMENT_STORE_OP_DONT_CARE;

	colorAttachment.initialLayout=VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout=VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef={};
	colorAttachmentRef.attachment=0;  // <- This works as an index.
	colorAttachmentRef.layout=VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription depthAttachment={};
	bool hasStencil;
	FsVulkanFindDepthStencilBufferFormat(depthAttachment.format,hasStencil,owner->physicalDevice);
	depthAttachment.samples=VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp=VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp=VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp=VK_ATTACHMENT_LOAD_OP_CLEAR; // DONT_CARE?
	depthAttachment.stencilStoreOp=VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout=VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout=VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef={};
	depthAttachmentRef.attachment=1;  // <- This works as an index.
	depthAttachmentRef.layout=VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	

	VkSubpassDescription subpass={};
	subpass.pipelineBindPoint=VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount=1;
	subpass.pColorAttachments=&colorAttachmentRef;
	subpass.pDepthStencilAttachment=&depthAttachmentRef;
	// According to the tutorial, "The index of the attachment in this array (pColorAttachments)
	// is directly referenced from the fragment shader with the 
	//    layout(location=0) out vec4 outColor
	// What does it mean?

	// The tutorial creates:
	//   Shaders,
	//   Render pass, and then
	//   Framebuffer.
	// It shouldn't have to be this order.  If framebuffer depended on shaders, Vulkan would be defected.
	// I must be able to create:
	//   Render pass, 
	//   Framebuffer, and then
	//   Shaders.
	// However, the above statement is bothering.  If render pass depends on the shaders, framebuffer depends
	// on the render pass, therefore framebuffer is shader-dependent.

	// As far as I understand, Vulkan has sub-passes as specified here, plus two implicit sub-passes 
	// that runs before and after my sub-pass.  My sub-pass must wait for the first implicit sub-pass to
	// run.  To ensure the order, dependency can be defined.
	// https://vulkan-tutorial.com/Drawing_a_triangle/Drawing/Rendering_and_presentation#page_Subpass_dependencies
	VkSubpassDependency dependency={};
	dependency.srcSubpass=VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass=0;
	dependency.srcStageMask=VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask=0;
	dependency.dstStageMask=VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask=VK_ACCESS_COLOR_ATTACHMENT_READ_BIT|VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	// I still don't fully understand the logic though.

	VkAttachmentDescription attachment[]={colorAttachment,depthAttachment};

	VkRenderPassCreateInfo renderPassInfo={};
	renderPassInfo.sType=VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount=sizeof(attachment)/sizeof(attachment[0]);
	renderPassInfo.pAttachments=attachment;
	renderPassInfo.subpassCount=1;
	renderPassInfo.pSubpasses=&subpass;
	renderPassInfo.dependencyCount=1;
	renderPassInfo.pDependencies=&dependency;
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

void FsVulkanContext::RenderTarget::CreateFramebuffer(void)
{
	int i=0;
	for(auto &f : frame)
	{
		std::vector <VkImageView> attachment;
		if(VK_NULL_HANDLE!=f.imageView)
		{
			attachment.push_back(f.imageView);
		}
		if(VK_NULL_HANDLE!=f.depthImageView)
		{
			attachment.push_back(f.depthImageView);
		}

		VkFramebufferCreateInfo info={};
		info.sType=VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		info.renderPass=renderPass;
		info.attachmentCount=attachment.size();
		info.pAttachments=attachment.data();
		info.width=f.imageExtent.width;
		info.height=f.imageExtent.height;
		info.layers=1;
		if(VK_SUCCESS!=vkCreateFramebuffer(owner->logicalDevice,&info,nullptr,&f.framebuffer))
		{
			printf("Failed to create a framebuffer.\n");
		}
		else
		{
			printf("Created framebuffer[%d]!\n",i);
		}
		++i;
	}
}

void FsVulkanContext::RenderTarget::CreateUniformPool(void)
{
	for(auto &f : frame)
	{
		VkDescriptorPoolSize uniformPoolSize[2]={{},{}};
		uniformPoolSize[0].type=VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uniformPoolSize[0].descriptorCount=UNIFORM_POOL_SIZE;
		uniformPoolSize[1].type=VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		uniformPoolSize[1].descriptorCount=UNIFORM_POOL_SIZE;

		VkDescriptorPoolCreateInfo uniformPoolInfo={};
		uniformPoolInfo.sType=VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		uniformPoolInfo.flags=0;
		uniformPoolInfo.poolSizeCount=sizeof(uniformPoolSize)/sizeof(uniformPoolSize[0]);
		uniformPoolInfo.pPoolSizes=uniformPoolSize;
		uniformPoolInfo.maxSets=UNIFORM_POOL_SIZE;
		if(VK_SUCCESS!=vkCreateDescriptorPool(owner->logicalDevice,&uniformPoolInfo,nullptr,&f.perFrameUniformPool))
		{
			printf("Failed to create a uniform pool.\n");
		}
	}
}

void FsVulkanContext::RenderTarget::CreateDepthBuffer(void)
{
	VkFormat fom;
	bool hasStencil;
	FsVulkanFindDepthStencilBufferFormat(fom,hasStencil,owner->physicalDevice);

	VkImageCreateInfo imageInfo={};
	imageInfo.sType=VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType=VK_IMAGE_TYPE_2D;
	imageInfo.extent.width=128; // Tentative. Actual value will be set for each frame.
	imageInfo.extent.height=128; // Tentative. Actual value will be set for each frame.
	imageInfo.extent.depth=1;
	imageInfo.mipLevels=1;
	imageInfo.arrayLayers=1; // ?
	imageInfo.format=fom;
	imageInfo.tiling=VK_IMAGE_TILING_OPTIMAL;
	imageInfo.initialLayout=VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage=VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	imageInfo.sharingMode=VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples=VK_SAMPLE_COUNT_1_BIT;
	imageInfo.flags=0;

	for(auto &f : frame)
	{
		imageInfo.extent.width=f.imageExtent.width;
		imageInfo.extent.height=f.imageExtent.height;
		if(VK_SUCCESS!=vkCreateImage(owner->logicalDevice,&imageInfo,nullptr,&f.depthImage))
		{
			printf("Cannot create image.\n");
			goto BAILOUT;
		}

		f.depthMemory=owner->AllocMemoryForImage(f.depthImage,VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		if(VK_NULL_HANDLE==f.depthMemory)
		{
			vkDestroyImage(owner->logicalDevice,f.depthImage,nullptr);
			f.depthImage=VK_NULL_HANDLE;
			goto BAILOUT;
		}
		vkBindImageMemory(owner->logicalDevice,f.depthImage,f.depthMemory,0);

		VkImageViewCreateInfo imageViewInfo={};
		imageViewInfo.sType=VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewInfo.image=f.depthImage;
		imageViewInfo.viewType=VK_IMAGE_VIEW_TYPE_2D;
		imageViewInfo.format=fom;
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
			goto BAILOUT;
		}

		{
			FsVulkanContext::OneTimeCommandGuard cmdBuf(*owner);

			uint32_t aspectFlag=VK_IMAGE_ASPECT_DEPTH_BIT;
			if(true==hasStencil)
			{
				aspectFlag|=VK_IMAGE_ASPECT_STENCIL_BIT;
			}

			owner->TransitionImageLayout(cmdBuf,
				f.depthImage,
				VK_IMAGE_LAYOUT_UNDEFINED,                       0,                           VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
				VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
				                                                 VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT|VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
				                                                                              VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
				aspectFlag);
		}
	}
BAILOUT:
	;
}

void FsVulkanContext::RenderTarget::CreateCommandBuffer(void)
{
	VkCommandBufferAllocateInfo commandBufferInfo={};
	commandBufferInfo.sType=VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferInfo.commandPool=owner->commandPool;
	commandBufferInfo.level=VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufferInfo.commandBufferCount=frame.size();

	std::vector <VkCommandBuffer> commandBuffer;
	commandBuffer.resize(frame.size());
	if(VK_SUCCESS!=vkAllocateCommandBuffers(owner->logicalDevice,&commandBufferInfo,commandBuffer.data()))
	{
		printf("Failed to create command buffers.\n");
	}
	for(long long int i=0; i<frame.size(); ++i)
	{
		frame[i].commandBuffer=commandBuffer[i];
	}
}

void FsVulkanContext::RenderTarget::DestroyRenderTarget(void)
{
	for(auto &f : frame)
	{
		f.DestroyAllPerFrameResource();

		if(VK_NULL_HANDLE!=f.framebuffer)
		{
			vkDestroyFramebuffer(owner->logicalDevice,f.framebuffer,nullptr);
		}
		f.framebuffer=VK_NULL_HANDLE;

		if(VK_NULL_HANDLE!=f.depthImageView)
		{
			vkDestroyImageView(owner->logicalDevice,f.depthImageView,nullptr);
		}
		f.depthImageView=VK_NULL_HANDLE;

		if(VK_NULL_HANDLE!=f.depthImage)
		{
			vkDestroyImage(owner->logicalDevice,f.depthImage,nullptr);
		}
		f.depthImage=VK_NULL_HANDLE;

		if(VK_NULL_HANDLE!=f.depthMemory)
		{
			vkFreeMemory(owner->logicalDevice,f.depthMemory,0);
		}
		f.depthMemory=VK_NULL_HANDLE;

		if(VK_NULL_HANDLE!=f.imageView)
		{
			vkDestroyImageView(owner->logicalDevice,f.imageView,nullptr);
		}
		f.imageView=VK_NULL_HANDLE;

		if(VK_NULL_HANDLE!=f.commandBuffer)
		{
			vkFreeCommandBuffers(owner->logicalDevice,owner->commandPool,1,&f.commandBuffer);
		}
		f.commandBuffer=VK_NULL_HANDLE;

		if(VK_NULL_HANDLE!=f.perFrameUniformPool)
		{
			vkDestroyDescriptorPool(owner->logicalDevice,f.perFrameUniformPool,nullptr);
		}
		f.perFrameUniformPool=VK_NULL_HANDLE;
	}

	if(VK_NULL_HANDLE!=renderPass)
	{
		vkDestroyRenderPass(owner->logicalDevice,renderPass,nullptr);
	}
	renderPass=VK_NULL_HANDLE;
}

////////////////////////////////////////////////////////////

FsVulkanContext::Buffer FsVulkanContext::Frame::CreatePerFrameVertexBuffer(long long int bufSize,const void *data)
{
	auto buf=owner->owner->CreateVertexBuffer(bufSize);
	if(VK_NULL_HANDLE!=buf.buf)
	{
		if(nullptr!=data)
		{
			owner->owner->TransferToDeviceMemory(buf.mem,bufSize,(void *)data);
		}
		perFrameBuffer.push_back(buf);
	}
	return buf;
}

FsVulkanContext::Buffer FsVulkanContext::Frame::CreatePerFrameUniformBuffer(long long int bufSize,const void *data)
{
	auto buf=owner->owner->CreateUniformBuffer(bufSize);
	if(VK_NULL_HANDLE!=buf.buf)
	{
		if(nullptr!=data)
		{
			owner->owner->TransferToDeviceMemory(buf.mem,bufSize,(void *)data);
		}
		perFrameBuffer.push_back(buf);
	}
	return buf;
}

VkDescriptorSet FsVulkanContext::Frame::CreatePerFrameUniformBinder(const Renderer &renderer,int uniformIdx,Buffer uniformBuf)
{
	auto uniformBinding=renderer.GetUniformBinding(uniformIdx);

	VkDescriptorSetLayout layout[1]={uniformBinding.layout};
	VkDescriptorSetAllocateInfo allocInfo={};
	allocInfo.sType=VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool=perFrameUniformPool;  // <- Different from FsVulkanContext::AllocSampler
	allocInfo.descriptorSetCount=1;
	allocInfo.pSetLayouts=layout;

	VkDescriptorSet descSet;
	if(VK_SUCCESS!=vkAllocateDescriptorSets(owner->owner->logicalDevice,&allocInfo,&descSet))
	{
		return VK_NULL_HANDLE;
	}

	owner->owner->UpdateUniform(descSet,uniformBuf,uniformBinding.binding);

	return descSet;
}

VkDescriptorSet FsVulkanContext::Frame::CreatePerFrameUniformBinder(const Renderer &renderer,int uniformIdx,long long int bufSize,const void *data)
{
	auto buf=CreatePerFrameUniformBuffer(bufSize,data);
	auto uniformBinding=renderer.GetUniformBinding(uniformIdx);

	VkDescriptorSetLayout layout[1]={uniformBinding.layout};
	VkDescriptorSetAllocateInfo allocInfo={};
	allocInfo.sType=VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool=perFrameUniformPool;  // <- Different from FsVulkanContext::AllocSampler
	allocInfo.descriptorSetCount=1;
	allocInfo.pSetLayouts=layout;

	VkDescriptorSet descSet;
	if(VK_SUCCESS!=vkAllocateDescriptorSets(owner->owner->logicalDevice,&allocInfo,&descSet))
	{
		return VK_NULL_HANDLE;
	}

	owner->owner->UpdateUniform(descSet,buf,uniformBinding.binding);

	return descSet;
}

VkDescriptorSet FsVulkanContext::Frame::CreatePerFrameSamplerBinder(const Renderer &renderer,int uniformIdx,const Texture2d &tex2d) const
{
	auto uniformBinding=renderer.GetUniformBinding(uniformIdx);

	VkDescriptorSetLayout layout[1]={uniformBinding.layout};
	VkDescriptorSetAllocateInfo allocInfo={};
	allocInfo.sType=VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool=perFrameUniformPool;  // <- Different from FsVulkanContext::AllocSampler
	allocInfo.descriptorSetCount=1;
	allocInfo.pSetLayouts=layout;

	VkDescriptorSet descSet;
	if(VK_SUCCESS!=vkAllocateDescriptorSets(owner->owner->logicalDevice,&allocInfo,&descSet))
	{
		return VK_NULL_HANDLE;
	}

	owner->owner->UpdateSampler(descSet,1,&tex2d,uniformBinding.binding);

	return descSet;
}

VkDescriptorSet FsVulkanContext::Frame::CreatePerFrameSamplerBinder(const Renderer &renderer,int uniformIdx,long long int nTex,const Texture2d tex2d[]) const
{
	auto uniformBinding=renderer.GetUniformBinding(uniformIdx);

	VkDescriptorSetLayout layout[1]={uniformBinding.layout};
	VkDescriptorSetAllocateInfo allocInfo={};
	allocInfo.sType=VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool=perFrameUniformPool;  // <- Different from FsVulkanContext::AllocSampler
	allocInfo.descriptorSetCount=1;
	allocInfo.pSetLayouts=layout;

	VkDescriptorSet descSet;
	if(VK_SUCCESS!=vkAllocateDescriptorSets(owner->owner->logicalDevice,&allocInfo,&descSet))
	{
		return VK_NULL_HANDLE;
	}

	owner->owner->UpdateSampler(descSet,nTex,tex2d,uniformBinding.binding);

	return descSet;
}

void FsVulkanContext::Frame::DestroyAllPerFrameResource(void)
{
	for(auto buf : perFrameBuffer)
	{
		vkDestroyBuffer(owner->owner->logicalDevice,buf.buf,nullptr);
		vkFreeMemory(owner->owner->logicalDevice,buf.mem,nullptr);
	}
	perFrameBuffer.clear();

	vkResetDescriptorPool(owner->owner->logicalDevice,perFrameUniformPool,0);
}

void FsVulkanContext::Frame::BeginCommandBuffer(void)
{
	VkCommandBufferBeginInfo info={};
	info.sType=VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	info.flags=VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
	info.pInheritanceInfo=nullptr;
	if(VK_SUCCESS!=vkBeginCommandBuffer(commandBuffer,&info))
	{
		printf("Failed to begin command-buffer recording!\n");
	}

	VkViewport viewport[1]={{}};
	viewport[0].x=0.0f;
	viewport[0].y=0.0f;
	viewport[0].width=(float)imageExtent.width;
	viewport[0].height=(float)imageExtent.height;
	viewport[0].minDepth=0.0f;
	viewport[0].maxDepth=1.0f;
	vkCmdSetViewport(commandBuffer,0,1,viewport);

	VkRect2D scissor[1]={{}};
	scissor[0].offset={0,0};
	scissor[0].extent=imageExtent;
	vkCmdSetScissor(commandBuffer,0,1,scissor);

	vkCmdSetLineWidth(commandBuffer,1.0f);
}

////////////////////////////////////////////////////////////

FsVulkanContext::Renderer::Renderer(RendererPool *targetPtr)
{
	this->owner=targetPtr;
	pipelineLayout=VK_NULL_HANDLE;
	pointListPipeline=VK_NULL_HANDLE;
	triangleListPipeline=VK_NULL_HANDLE;
	lineListPipeline=VK_NULL_HANDLE;
}
void FsVulkanContext::Renderer::AddUniformBinding(uint32_t binding,VkDescriptorSetLayout layout,long long int length)
{
	UniformBinding b;
	b.binding=binding;
	b.layout=layout;
	b.length=length;
	uniformBinding.push_back(b);
}
void FsVulkanContext::Renderer::CleanUp(void)
{
	if(VK_NULL_HANDLE!=triangleListPipeline)
	{
		vkDestroyPipeline(owner->owner->logicalDevice,triangleListPipeline,nullptr);
	}
	if(VK_NULL_HANDLE!=lineListPipeline)
	{
		vkDestroyPipeline(owner->owner->logicalDevice,lineListPipeline,nullptr);
	}
	if(VK_NULL_HANDLE!=pointListPipeline)
	{
		vkDestroyPipeline(owner->owner->logicalDevice,pointListPipeline,nullptr);
	}
	if(VK_NULL_HANDLE!=pipelineLayout)
	{
		vkDestroyPipelineLayout(owner->owner->logicalDevice,pipelineLayout,nullptr);
	}
	for(auto uniform : uniformBinding)
	{
		vkDestroyDescriptorSetLayout(owner->owner->logicalDevice,uniform.layout,nullptr);
	}
	uniformBinding.clear();
	pointListPipeline=VK_NULL_HANDLE;
	triangleListPipeline=VK_NULL_HANDLE;
	lineListPipeline=VK_NULL_HANDLE;
	pipelineLayout=VK_NULL_HANDLE;
}

////////////////////////////////////////////////////////////

FsVulkanContext FsVulkanContext::context;

/* static */ FsVulkanContext &FsVulkanContext::GetContext(void)
{
	return context;
}

FsVulkanContext::FsVulkanContext() : primaryRendererPool(this)
{
	primarySwapChain.owner=this;

	inst=VK_NULL_HANDLE;
	physicalDevice=VK_NULL_HANDLE;
	surface=VK_NULL_HANDLE;

	imageAvailableSemaphore=VK_NULL_HANDLE;
	renderFinishedSemaphore=VK_NULL_HANDLE;

	primarySwapChain.renderPass=VK_NULL_HANDLE;
}

FsVulkanContext::~FsVulkanContext()
{
	CleanUp();
}

void FsVulkanContext::CleanUp(void)
{
	if(VK_NULL_HANDLE!=logicalDevice)
	{
		vkDeviceWaitIdle(logicalDevice);

		stock.CleanUp(*this);

		vkDestroySemaphore(logicalDevice,imageAvailableSemaphore,nullptr);
		vkDestroySemaphore(logicalDevice,renderFinishedSemaphore,nullptr);

		primaryRendererPool.DestroyRenderer();
		primarySwapChain.DestroyRenderTarget();
		primarySwapChain.DestroySwapChain();

		vkDestroyCommandPool(logicalDevice,commandPool,nullptr);
		commandPool=VK_NULL_HANDLE;

		vkDestroyDescriptorPool(logicalDevice,uniformPool,nullptr);
		uniformPool=VK_NULL_HANDLE;

		vkDestroyDevice(logicalDevice,nullptr);
		logicalDevice=VK_NULL_HANDLE;

		vkDestroySurfaceKHR(inst,surface,nullptr);
		vkDestroyInstance(inst,nullptr);
	}

	// For perfectly no error:
	// Must keep track of created pipelines (programs) and destroy them explicitly.
}

void FsVulkanContext::CreateInstance(int useValidationLayer)
{
	this->useValidationLayer=useValidationLayer;

	VkResult err;

	VkApplicationInfo appInfo={};
	appInfo.sType=VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext=nullptr;
	appInfo.pApplicationName="FsVulkanWindow Application";
	appInfo.applicationVersion=VK_MAKE_VERSION(18,06,16);
	appInfo.pEngineName="YSFLIGHT"; // "What? Shut up. Don't ask again.";
	appInfo.engineVersion=VK_MAKE_VERSION(0,0,0);
	appInfo.apiVersion=VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo={};
	VkDebugUtilsMessengerCreateInfoEXT debugCallBackInfo={};
	createInfo.sType=VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pNext=nullptr;
	createInfo.pApplicationInfo=&appInfo;

	auto instExt=FsVulkanEnumerateInstanceExtension();
	bool surfaceExtFound=false,platformSurfaceExtFound;
	if(instExt.end()!=std::find(instExt.begin(),instExt.end(),std::string(VK_KHR_SURFACE_EXTENSION_NAME)))
	{
		surfaceExtFound=true;
	}
	if(instExt.end()!=std::find(instExt.begin(),instExt.end(),std::string(FsVulkanGetPlatformSurfacExtensionName())))
	{
		platformSurfaceExtFound=true;
	}
	if(true!=surfaceExtFound || true!=platformSurfaceExtFound)
	{
		printf("Surface Extension and/or Platform Surface Extension not found.\n");
		printf("Maybe the end of the world.\n");
	}

	auto instExtPtr=FsVulkanSelectInstanceExtension(instExt);
	createInfo.enabledExtensionCount=instExtPtr.size();
	createInfo.ppEnabledExtensionNames=instExtPtr.data();

	auto allValidationLayer=FsVulkanEnumerateValidationLayer();
	auto allValidationLayerPtr=FsVulkanSelectValidationLayer(allValidationLayer);

	createInfo.enabledLayerCount=0;
	if(0!=useValidationLayer)
	{
		createInfo.enabledLayerCount=allValidationLayerPtr.size();
		createInfo.ppEnabledLayerNames=allValidationLayerPtr.data();
		if(0==allValidationLayerPtr.size())
		{
			printf("Validation Layer not found.\n");
			printf("Going without the layer.\n");
		}
		else
		{
			debugCallBackInfo.sType=VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			debugCallBackInfo.pNext=NULL;
			debugCallBackInfo.flags=0;
			debugCallBackInfo.messageSeverity=
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			debugCallBackInfo.messageType=
				VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			debugCallBackInfo.pfnUserCallback=FsVulkanDebugMessageCallBack;
			debugCallBackInfo.pUserData=nullptr;
			createInfo.pNext=&debugCallBackInfo;
		}
	}

	VkResult res=vkCreateInstance(&createInfo,NULL,&inst);
	if(VK_SUCCESS!=res)
	{
		printf("Vulkan Instance could not be created.\n");
		printf("It's the end of the world.\n");
	}
}

void FsVulkanContext::SetPlatformSpecificSurface(VkSurfaceKHR surface)
{
	this->surface=surface;
}

void FsVulkanContext::CreatePhysicalDevice(void)
{
	auto allPhysDev=FsVulkanEnumeratePhysicalDevice(inst);
	physicalDevice=FsVulkanSelectPhysicalDevice(allPhysDev,surface);
	if(VK_NULL_HANDLE==physicalDevice)
	{
		printf("No Vulkan Physical Device is found.\n");
		printf("Most likely to be the end of the world.\n");
	}
}

void FsVulkanContext::CreateLogicalDevice(void)
{
	uint32_t graphicsQueueIndex=0,presentQueueIndex=0;
	FsVulkanGetQueueFamilyIndex(graphicsQueueIndex,presentQueueIndex,physicalDevice,surface);
	sameQueue=(graphicsQueueIndex==presentQueueIndex);

	std::vector <VkDeviceQueueCreateInfo> queueCreateInfo;
	if(true==sameQueue)
	{
		const uint32_t queueFamilyIndex[]={graphicsQueueIndex};
		const float queuePriority[1]={1};

		VkDeviceQueueCreateInfo info={};
		info.sType=VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		info.queueFamilyIndex=graphicsQueueIndex;
		info.queueCount=1;
		info.pQueuePriorities=queuePriority;
		queueCreateInfo.push_back(info);
	}
	else
	{
		const uint32_t queueFamilyIndex[]={graphicsQueueIndex,presentQueueIndex};
		const float queuePriority[1]={1};
		for(auto idx : queueFamilyIndex)
		{
			VkDeviceQueueCreateInfo info={};
			info.sType=VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			info.queueFamilyIndex=idx;
			info.queueCount=1;
			info.pQueuePriorities=queuePriority;
			queueCreateInfo.push_back(info);
		}
	}

	VkPhysicalDeviceFeatures physDevFeature={};
	physDevFeature.wideLines=VK_TRUE;
	physDevFeature.largePoints=VK_TRUE;
	physDevFeature.samplerAnisotropy=VK_TRUE;
	physDevFeature.depthBounds=VK_TRUE;
	auto devExt=FsVulkanEnumerateDeviceExtension(physicalDevice);
	auto devExtPtr=FsVulkanSelectDeviceExtension(devExt);

	VkDeviceCreateInfo logDevCreateInfo={};
	logDevCreateInfo.sType=VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	logDevCreateInfo.queueCreateInfoCount=(int)queueCreateInfo.size();
	logDevCreateInfo.pQueueCreateInfos=queueCreateInfo.data();
	logDevCreateInfo.pEnabledFeatures=&physDevFeature;
	logDevCreateInfo.enabledExtensionCount=devExtPtr.size();
	logDevCreateInfo.ppEnabledExtensionNames=devExtPtr.data();

	auto allValidationLayer=FsVulkanEnumerateValidationLayer();
	auto allValidationLayerPtr=FsVulkanSelectValidationLayer(allValidationLayer);
	if(0!=this->useValidationLayer)
	{
		logDevCreateInfo.enabledLayerCount=allValidationLayerPtr.size();
		logDevCreateInfo.ppEnabledLayerNames=allValidationLayerPtr.data();
	}
	else
	{
		logDevCreateInfo.enabledLayerCount=0;
	}

	auto res=vkCreateDevice(physicalDevice,&logDevCreateInfo,nullptr,&logicalDevice);
	if(VK_SUCCESS!=res)
	{
		printf("Could not create a logical device.\n");
		printf("Must be the end of the world.\n");
	}

	vkGetDeviceQueue(logicalDevice,graphicsQueueIndex,0,&graphicsQueue);
	if(true==sameQueue)
	{
		presentQueue=graphicsQueue;
	}
	else
	{
		vkGetDeviceQueue(logicalDevice,presentQueueIndex,0,&presentQueue);
	}

	VkCommandPoolCreateInfo poolInfo={};
	poolInfo.sType=VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags=VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex=FsVulkanGetGraphicsQueueFamilyIndex(physicalDevice);

	if(VK_SUCCESS!=vkCreateCommandPool(logicalDevice,&poolInfo,nullptr,&commandPool))
	{
		printf("Failed to create a command pool.\n");
		printf("Very likely to be the end of the world.\n");
	}

	{
		VkDescriptorPoolSize uniformPoolSize[2]={{},{}};
		uniformPoolSize[0].type=VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uniformPoolSize[0].descriptorCount=UNIFORM_POOL_SIZE;
		uniformPoolSize[1].type=VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		uniformPoolSize[1].descriptorCount=UNIFORM_POOL_SIZE;

		VkDescriptorPoolCreateInfo uniformPoolInfo={};
		uniformPoolInfo.sType=VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		uniformPoolInfo.flags=VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		uniformPoolInfo.poolSizeCount=sizeof(uniformPoolSize)/sizeof(uniformPoolSize[0]);
		uniformPoolInfo.pPoolSizes=uniformPoolSize;
		uniformPoolInfo.maxSets=UNIFORM_POOL_SIZE;
		if(VK_SUCCESS!=vkCreateDescriptorPool(logicalDevice,&uniformPoolInfo,nullptr,&uniformPool))
		{
			printf("Failed to create a uniform pool.\n");
		}
	}
}

void FsVulkanContext::CreateSemaphore(void)
{
	VkSemaphoreCreateInfo semaphorInfo={};
	semaphorInfo.sType=VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	// I hope it at least doesn't fail.
	vkCreateSemaphore(logicalDevice,&semaphorInfo,nullptr,&imageAvailableSemaphore);
	vkCreateSemaphore(logicalDevice,&semaphorInfo,nullptr,&renderFinishedSemaphore);
}

void FsVulkanContext::CreateStockResoruce(void)
{
	stock.Create(*this);
}


void FsVulkanContext::SubmitCommandBuffer(VkCommandBuffer cmdBuf,VkSemaphore waitSemaphoreIn,VkSemaphore signalSemaphoreIn)
{
	std::vector <VkCommandBuffer> cmdBufArray={cmdBuf};
	SubmitCommandBuffer(cmdBufArray,waitSemaphoreIn,signalSemaphoreIn);
}

void FsVulkanContext::SubmitCommandBuffer(const std::vector <VkCommandBuffer> &cmdBuf,VkSemaphore waitSemaphoreIn,VkSemaphore signalSemaphoreIn)
{
	VkSubmitInfo submitInfo={};
	submitInfo.sType=VK_STRUCTURE_TYPE_SUBMIT_INFO;

	if(VK_NULL_HANDLE==waitSemaphoreIn)
	{
		waitSemaphoreIn=imageAvailableSemaphore; // Didn't I acquire in ReadyFrame?
	}
	if(VK_NULL_HANDLE==signalSemaphoreIn)
	{
		signalSemaphoreIn=renderFinishedSemaphore;
	}

	VkSemaphore waitSemaphore[]={waitSemaphoreIn};
	VkPipelineStageFlags waitStage[]={VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submitInfo.waitSemaphoreCount=1;
	submitInfo.pWaitSemaphores=waitSemaphore;
	submitInfo.pWaitDstStageMask=waitStage;

	submitInfo.commandBufferCount=cmdBuf.size();
	submitInfo.pCommandBuffers=cmdBuf.data();

	VkSemaphore signalSemaphore[]={signalSemaphoreIn};
	submitInfo.signalSemaphoreCount=1;
	submitInfo.pSignalSemaphores=signalSemaphore;

	if(VK_SUCCESS!=vkQueueSubmit(graphicsQueue,1,&submitInfo,VK_NULL_HANDLE))
	{
		printf("Failed to submit a queue.\n");
	}
}

void FsVulkanContext::ShowFrame(const Frame &frame)
{
	VkPresentInfoKHR presentInfo={};
	presentInfo.sType=VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount=1;
	presentInfo.pWaitSemaphores=&renderFinishedSemaphore;

	VkSwapchainKHR swapChain[]={this->primarySwapChain.swapChain};
	uint32_t imageIndex[]={frame.imageIndex};
	VkResult resArray[1];
	presentInfo.swapchainCount=1;
	presentInfo.pSwapchains=swapChain;
	presentInfo.pImageIndices=imageIndex;
	presentInfo.pResults=resArray;

	vkQueuePresentKHR(presentQueue,&presentInfo);
}

FsVulkanContext::Buffer FsVulkanContext::CreateVertexBuffer(long long int bufSize) const
{
	Buffer buf;
	buf.length=bufSize;

	VkBufferCreateInfo vtxBufInfo={};
	vtxBufInfo.sType=VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	vtxBufInfo.size=bufSize;
	vtxBufInfo.usage=VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;   // <- It says "vertex"
	vtxBufInfo.sharingMode=VK_SHARING_MODE_EXCLUSIVE;
	vkCreateBuffer(logicalDevice,&vtxBufInfo,nullptr,&buf.buf);

	buf.mem=AllocMemoryForBuffer(buf.buf,VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	if(VK_NULL_HANDLE!=buf.mem)
	{
		vkBindBufferMemory(logicalDevice,buf.buf,buf.mem,0);
		return buf;
	}

	printf("Failed to create a vertex buffer (or allocate memory).\n");
	vkDestroyBuffer(logicalDevice,buf.buf,nullptr);
	buf.buf=VK_NULL_HANDLE;
	return buf;
}

void FsVulkanContext::DestroyBuffer(Buffer &buf)
{
	if(VK_NULL_HANDLE!=buf.buf)
	{
		vkDestroyBuffer(logicalDevice,buf.buf,nullptr);
	}
	if(VK_NULL_HANDLE!=buf.mem)
	{
		vkFreeMemory(logicalDevice,buf.mem,nullptr);
	}
	buf.buf=VK_NULL_HANDLE;
	buf.mem=VK_NULL_HANDLE;
}

FsVulkanContext::Buffer FsVulkanContext::CreateUniformBuffer(long long int bufSize) const
{
	Buffer buf;
	buf.length=bufSize;

	VkBufferCreateInfo vtxBufInfo={};
	vtxBufInfo.sType=VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	vtxBufInfo.size=bufSize;
	vtxBufInfo.usage=VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;   // <- It says "uniform"
	vtxBufInfo.sharingMode=VK_SHARING_MODE_EXCLUSIVE;
	vkCreateBuffer(logicalDevice,&vtxBufInfo,nullptr,&buf.buf);

	buf.mem=AllocMemoryForBuffer(buf.buf,VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	if(VK_NULL_HANDLE!=buf.mem)
	{
		vkBindBufferMemory(logicalDevice,buf.buf,buf.mem,0);
		return buf;
	}

	printf("Failed to create a uniform buffer (or allocate memory).\n");
	vkDestroyBuffer(logicalDevice,buf.buf,nullptr);
	buf.buf=VK_NULL_HANDLE;
	return buf;
}

FsVulkanContext::Texture2d FsVulkanContext::CreateTexture2d(int wid,int hei,const unsigned char rgba[]) const
{
	const long long int bufSize=wid*hei*4;

	VkBuffer stagingBuf;

	VkBufferCreateInfo stagingBufInfo={};
	stagingBufInfo.sType=VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	stagingBufInfo.size=bufSize;
	stagingBufInfo.usage=VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	stagingBufInfo.sharingMode=VK_SHARING_MODE_EXCLUSIVE;
	vkCreateBuffer(logicalDevice,&stagingBufInfo,nullptr,&stagingBuf);

	auto stagingBufMem=AllocMemoryForBuffer(stagingBuf,VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	if(VK_NULL_HANDLE!=stagingBufMem)
	{
		vkBindBufferMemory(logicalDevice,stagingBuf,stagingBufMem,0);
	}

	{
		void *data;
		vkMapMemory(logicalDevice,stagingBufMem,0,bufSize,0,&data);
		memcpy(data,rgba,bufSize);
		vkUnmapMemory(logicalDevice,stagingBufMem);
	}

	Texture2d tex;
	tex.image=VK_NULL_HANDLE;
	tex.mem=VK_NULL_HANDLE;

	VkImageCreateInfo imageInfo={};
	imageInfo.sType=VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType=VK_IMAGE_TYPE_2D;
	imageInfo.extent.width=wid;
	imageInfo.extent.height=hei;
	imageInfo.extent.depth=1;
	imageInfo.mipLevels=1;
	imageInfo.arrayLayers=1; // ?
	imageInfo.format=VK_FORMAT_R8G8B8A8_UNORM;
	imageInfo.tiling=VK_IMAGE_TILING_OPTIMAL;
	imageInfo.initialLayout=VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage=VK_IMAGE_USAGE_TRANSFER_DST_BIT|VK_IMAGE_USAGE_SAMPLED_BIT;
	imageInfo.sharingMode=VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples=VK_SAMPLE_COUNT_1_BIT;
	imageInfo.flags=0;
	if(VK_SUCCESS!=vkCreateImage(logicalDevice,&imageInfo,nullptr,&tex.image))
	{
		printf("Cannot create image.\n");
		goto BAILOUT;
	}

	tex.mem=AllocMemoryForImage(tex.image,VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	if(VK_NULL_HANDLE==tex.mem)
	{
		vkDestroyImage(logicalDevice,tex.image,nullptr);
		tex.image=VK_NULL_HANDLE;
		goto BAILOUT;
	}
	vkBindImageMemory(logicalDevice,tex.image,tex.mem,0);


	{
		OneTimeCommandGuard cmdBuf(*this);

		TransitionImageLayout(cmdBuf,
			tex.image,
			VK_IMAGE_LAYOUT_UNDEFINED,           0,                           VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,VK_ACCESS_TRANSFER_WRITE_BIT,VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_IMAGE_ASPECT_COLOR_BIT);

		VkBufferImageCopy copy={};
		copy.bufferOffset=0;
		copy.bufferRowLength=0;
		copy.bufferImageHeight=0;
		copy.imageSubresource.aspectMask=VK_IMAGE_ASPECT_COLOR_BIT;
		copy.imageSubresource.mipLevel=0;
		copy.imageSubresource.baseArrayLayer=0;
		copy.imageSubresource.layerCount=1;
		copy.imageOffset={0,0,0};
		copy.imageExtent={(uint32_t)wid,(uint32_t)hei,1};
		vkCmdCopyBufferToImage(cmdBuf,stagingBuf,tex.image,VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,1,&copy);

		TransitionImageLayout(cmdBuf,
			tex.image,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,    VK_ACCESS_TRANSFER_WRITE_BIT,VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,VK_ACCESS_SHADER_READ_BIT,   VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			VK_IMAGE_ASPECT_COLOR_BIT);
	}

	{
		VkImageViewCreateInfo imageViewInfo={};
		imageViewInfo.sType=VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewInfo.image=tex.image;
		imageViewInfo.viewType=VK_IMAGE_VIEW_TYPE_2D;
		imageViewInfo.format=VK_FORMAT_R8G8B8A8_UNORM;
		imageViewInfo.subresourceRange.aspectMask=VK_IMAGE_ASPECT_COLOR_BIT;
		imageViewInfo.subresourceRange.baseMipLevel=0;
		imageViewInfo.subresourceRange.levelCount=1;
		imageViewInfo.subresourceRange.baseArrayLayer=0;
		imageViewInfo.subresourceRange.layerCount=1;
		if(VK_SUCCESS!=vkCreateImageView(logicalDevice,&imageViewInfo,nullptr,&tex.imageView))
		{
			printf("Failed to create an image view.\n");
			printf("I'm exhausted.\n");
		}
	}

	{
		VkSamplerCreateInfo samplerInfo={};
		samplerInfo.sType=VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter=VK_FILTER_LINEAR;
		samplerInfo.minFilter=VK_FILTER_LINEAR;
		samplerInfo.addressModeU=VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV=VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW=VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable=VK_TRUE;
		samplerInfo.maxAnisotropy=16;
		samplerInfo.borderColor=VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates=VK_FALSE;
		samplerInfo.compareEnable=VK_FALSE;
		samplerInfo.compareOp=VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode=VK_SAMPLER_MIPMAP_MODE_LINEAR;
		if(VK_SUCCESS!=vkCreateSampler(logicalDevice,&samplerInfo,nullptr,&tex.sampler))
		{
			printf("Failed to create a sampler.\n");
			printf("I'm tired.\n");
		}
	}

BAILOUT:
	vkDestroyBuffer(logicalDevice,stagingBuf,nullptr);
	vkFreeMemory(logicalDevice,stagingBufMem,nullptr);
	return tex;
}

void FsVulkanContext::DestroyTexture2d(Texture2d tex2d) const
{
	tex2d.CleanUp(logicalDevice);
}

FsVulkanContext::Uniform FsVulkanContext::AllocUniform(VkDescriptorSetLayout layoutIn) const
{
	VkDescriptorSetLayout layout[1]={layoutIn};
	VkDescriptorSetAllocateInfo allocInfo={};
	allocInfo.sType=VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool=uniformPool;
	allocInfo.descriptorSetCount=1;
	allocInfo.pSetLayouts=layout;

	Uniform uniform;
	if(VK_SUCCESS!=vkAllocateDescriptorSets(logicalDevice,&allocInfo,&uniform.descSet))
	{
		uniform.descPool=VK_NULL_HANDLE;
		uniform.descSet=VK_NULL_HANDLE;
	}
	uniform.descPool=uniformPool;
	return uniform;
}

void FsVulkanContext::UpdateUniform(VkDescriptorSet descSet,Buffer uni,uint32_t binding)
{
	VkDescriptorBufferInfo bufferInfo={};
	bufferInfo.buffer=uni.buf;
	bufferInfo.offset=0;
	bufferInfo.range=uni.length;

	VkWriteDescriptorSet write[1]={{}};
	write[0].sType=VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write[0].dstSet=descSet;
	write[0].dstBinding=binding;
	write[0].dstArrayElement=0;
	write[0].descriptorType=VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	write[0].descriptorCount=1;
	write[0].pBufferInfo=&bufferInfo;

	vkUpdateDescriptorSets(logicalDevice,1,write,0,nullptr);
}

FsVulkanContext::Sampler FsVulkanContext::AllocSampler(VkDescriptorSetLayout layoutIn) const
{
	VkDescriptorSetLayout layout[1]={layoutIn};
	VkDescriptorSetAllocateInfo allocInfo={};
	allocInfo.sType=VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool=uniformPool;
	allocInfo.descriptorSetCount=1;
	allocInfo.pSetLayouts=layout;

	Sampler uniform;
	if(VK_SUCCESS!=vkAllocateDescriptorSets(logicalDevice,&allocInfo,&uniform.descSet))
	{
		uniform.descPool=VK_NULL_HANDLE;
		uniform.descSet=VK_NULL_HANDLE;
	}
	uniform.descPool=uniformPool;
	return uniform;
}

void FsVulkanContext::UpdateSampler(VkDescriptorSet descSet,long long int nTex,const Texture2d tex2d[],uint32_t binding)
{
	std::vector <VkDescriptorImageInfo> imageInfo;
	for(int i=0; i<nTex; ++i)
	{
		VkDescriptorImageInfo info={};
		info.imageLayout=VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		info.imageView=tex2d[i].imageView;
		info.sampler=tex2d[i].sampler;
		imageInfo.push_back(info);
	}

	VkWriteDescriptorSet write[1]={{}};
	write[0].sType=VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write[0].dstSet=descSet;
	write[0].dstBinding=binding;
	write[0].dstArrayElement=0;
	write[0].descriptorType=VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	write[0].descriptorCount=imageInfo.size();
	write[0].pImageInfo=imageInfo.data();

	vkUpdateDescriptorSets(logicalDevice,1,write,0,nullptr);
}

void FsVulkanContext::FreeUniformOrSampler(DescriptorSetAndPool &setAndPool)
{
	vkFreeDescriptorSets(logicalDevice,setAndPool.descPool,1,&setAndPool.descSet);
	setAndPool.descSet=VK_NULL_HANDLE;
}

void FsVulkanContext::TransferToDeviceMemory(VkDeviceMemory mem,long long int len,const void *src) const
{
	void *dst;
	vkMapMemory(logicalDevice,mem,0,VK_WHOLE_SIZE,0,&dst);
	memcpy(dst,src,len);
	vkUnmapMemory(logicalDevice,mem);
}

VkDeviceMemory FsVulkanContext::AllocMemoryForBuffer(VkBuffer buf,const uint32_t memPropBit) const
{
	// Need to find a memory type that can be used as a vertex buffer.
	VkMemoryRequirements memReq;
	vkGetBufferMemoryRequirements(logicalDevice,buf,&memReq);
	return AllocMemoryForMemoryRequirement(memReq,memPropBit);
}

VkDeviceMemory FsVulkanContext::AllocMemoryForImage(VkImage img,const uint32_t memPropBit) const
{
	VkMemoryRequirements memReq;
	vkGetImageMemoryRequirements(logicalDevice,img,&memReq);
	return AllocMemoryForMemoryRequirement(memReq,memPropBit);
}

VkDeviceMemory FsVulkanContext::AllocMemoryForMemoryRequirement(const VkMemoryRequirements &memReq,const uint32_t memPropBit) const
{
	VkPhysicalDeviceMemoryProperties memProp;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice,&memProp);

	auto memTypeIndex=FindMatchingMemoryType(memReq,memProp,memPropBit);
	if(FsVulkan_INVALID_INDEX!=memTypeIndex)
	{
		VkDeviceMemory mem;
		VkMemoryAllocateInfo allocInfo={};
		allocInfo.sType=VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize=memReq.size;
		allocInfo.memoryTypeIndex=memTypeIndex;
		if(VK_SUCCESS==vkAllocateMemory(logicalDevice,&allocInfo,nullptr,&mem))
		{
			return mem;
		}
	}
	return VK_NULL_HANDLE;
}

void FsVulkanContext::TransitionImageLayout(
	    VkCommandBuffer cmdBuf,
	    VkImage img,
	    VkImageLayout prvLayout,const uint32_t prvAccess,const uint32_t prvStage,
	    VkImageLayout newLayout,const uint32_t newAccess,const uint32_t newStage,
	    uint32_t aspectFlag) const
{
	VkImageMemoryBarrier barrier={};
	barrier.sType=VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.image=img;

	barrier.subresourceRange.baseMipLevel=0;
	barrier.subresourceRange.levelCount=1;
	barrier.subresourceRange.baseArrayLayer=0;
	barrier.subresourceRange.layerCount=1;

	barrier.oldLayout=prvLayout;
	barrier.srcQueueFamilyIndex=VK_QUEUE_FAMILY_IGNORED;
	barrier.srcAccessMask=prvAccess;
	barrier.newLayout=newLayout;
	barrier.dstQueueFamilyIndex=VK_QUEUE_FAMILY_IGNORED;
	barrier.dstAccessMask=newAccess;

	barrier.subresourceRange.aspectMask=aspectFlag;

	vkCmdPipelineBarrier(cmdBuf,prvStage,newStage,0,0,nullptr,0,nullptr,1,&barrier);
}

uint32_t FsVulkanContext::FindMatchingMemoryType(
	    const VkMemoryRequirements &memReq,
	    const VkPhysicalDeviceMemoryProperties &memProp,
	    const uint32_t memPropBit) const
{
	// What's memoryTypeBit (typeFilter in the tutorial)???
	// typeFilter:
	// For example:
	//    0000 0000 0000 0000  0000 0000 0001 0000
	// If bit 4 is on, index 4 of memoryType in physical device memory property is good for the purpose.

	for(uint32_t i=0; i<memProp.memoryTypeCount; ++i)
	{
		if(0!=(memReq.memoryTypeBits & (1<<i)) && (memProp.memoryTypes[i].propertyFlags & memPropBit)==memPropBit)
		{
			return i;
		}
	}
	return FsVulkan_INVALID_INDEX;
}


VkShaderModule FsVulkanContext::CreateShaderModule(long long int len,const unsigned char spirv[],const char shaderName[])
{
	VkShaderModuleCreateInfo info={};
	info.sType=VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	info.codeSize=len;
	info.pCode=(const uint32_t *)spirv;

	VkShaderModule shader=VK_NULL_HANDLE;
	if(VK_SUCCESS!=vkCreateShaderModule(logicalDevice,&info,nullptr,&shader))
	{
		printf("Failed to create a shader.\n");
	}
	else
	{
		printf("Created a shader module [%s]!\n",shaderName);
	}

	return shader;
}

FsVulkanContext &FsVulkanGetContext(void)
{
	return FsVulkanContext::GetContext();
}

std::vector <std::string> FsVulkanEnumerateInstanceExtension(void)
{
	VkResult err;
	uint32_t nExtProp;
	std::vector <std::string> allExt;

	err=vkEnumerateInstanceExtensionProperties(nullptr,&nExtProp,nullptr);
	if(VK_SUCCESS==err && 0<nExtProp)
	{
		std::vector <VkExtensionProperties> allExtProp;

		allExtProp.resize(nExtProp);

		err=vkEnumerateInstanceExtensionProperties(nullptr,&nExtProp,allExtProp.data());
		if(VK_SUCCESS==err)
		{
			for(auto &extProp : allExtProp)
			{
				allExt.push_back(extProp.extensionName);
				printf("Extension: %s\n",allExt.back().c_str());
			}
		}
	}

	if(0==allExt.size())
	{
		printf("Vulkan Extensions not found.\n");
	}

	return allExt;
}

std::vector <const char *> FsVulkanSelectInstanceExtension(const std::vector <std::string> &foundExt)
{
	std::vector <const char *> instExtPtr;
	for(auto &ext : foundExt)
	{
		// Sample code demo.c for whatever reason selects only these extensions.  I don't understand why.
		if(ext==std::string(VK_KHR_SURFACE_EXTENSION_NAME) ||
		   ext==std::string(FsVulkanGetPlatformSurfacExtensionName()) ||
		   ext==std::string(VK_EXT_DEBUG_REPORT_EXTENSION_NAME) ||
		   ext==std::string(VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
		{
			instExtPtr.push_back(ext.data());
			printf("Selected Extension: %s\n",ext.data());
		}
	}
	return instExtPtr;
}

std::vector <std::string> FsVulkanEnumerateValidationLayer(void)
{
	std::vector <std::string> allValidationLayer;
	VkResult err;
	uint32_t nValidationLayer;
	err=vkEnumerateInstanceLayerProperties(&nValidationLayer,nullptr);
	if(VK_SUCCESS==err)
	{
		std::vector <VkLayerProperties> allLayerProp;
		allLayerProp.resize(nValidationLayer);
        err=vkEnumerateInstanceLayerProperties(&nValidationLayer,allLayerProp.data());
        if(VK_SUCCESS==err)
        {
			for(auto &layerProp : allLayerProp)
			{
				allValidationLayer.push_back(layerProp.layerName);
				printf("ValidationLayer: %s\n",layerProp.layerName);
			}
		}
	}
	return allValidationLayer;
}

std::vector <const char *> FsVulkanSelectValidationLayer(const std::vector <std::string> &foundLayer)
{
	std::vector <const char *> allLayer;
	for(auto &layer : foundLayer)
	{
		if(layer==std::string("VK_LAYER_LUNARG_standard_validation") ||
		   layer==std::string("MoltenVK"))
		{
			allLayer.push_back(layer.data());
		}
	}
	return allLayer;
}

std::vector <VkPhysicalDevice> FsVulkanEnumeratePhysicalDevice(VkInstance inst)
{
	std::vector <VkPhysicalDevice> allPhysDev;
	uint32_t numPhysDev=0;
	vkEnumeratePhysicalDevices(inst,&numPhysDev,nullptr);
	if(0<numPhysDev)
	{
		allPhysDev.resize(numPhysDev);
		vkEnumeratePhysicalDevices(inst,&numPhysDev,allPhysDev.data());
		for(auto &dev : allPhysDev)
		{
			VkPhysicalDeviceProperties prop;
			vkGetPhysicalDeviceProperties(dev,&prop);
			printf("PyhsDev: %s",prop.deviceName);
			if(VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU==prop.deviceType)
			{
				printf("[DiscreteGPU]");
			}
			else if(VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU==prop.deviceType)
			{
				printf("[IntegratedGPU]");
			}
			else if(VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU==prop.deviceType)
			{
				printf("[VirtualGPU]");
			}
			else if(VK_PHYSICAL_DEVICE_TYPE_CPU==prop.deviceType)
			{
				printf("[CPU]");
			}
			else
			{
				printf("[UnknownDeviceType]");
			}
			printf("\n");
		}
	}
	return allPhysDev;
}

VkPhysicalDevice FsVulkanSelectPhysicalDevice(const std::vector <VkPhysicalDevice> &allPhysDev,VkSurfaceKHR surface)
{
	const char *const allRequiredExt[]=
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	int bestScore=0;
	VkPhysicalDevice bestDev=VK_NULL_HANDLE;
	for(auto &dev : allPhysDev)
	{
		// Check queues are available
		uint32_t graphicsQueueIndex=0,presentQueueIndex=0;
		if(true!=FsVulkanGetQueueFamilyIndex(graphicsQueueIndex,presentQueueIndex,dev,surface))
		{
			continue;
		}

		// Check extension availability
		{
			bool extMissing=false;
			uint32_t nDevExt;
			auto devExt=FsVulkanEnumerateDeviceExtension(dev);
			for(auto reqExt : allRequiredExt)
			{
				bool found=false;
				for(auto &ext : devExt)
				{
					if(std::string(ext)==std::string(reqExt))
					{
						found=true;
						break;
					}
				}
				if(true!=found)
				{
					extMissing=true;
					break;
				}
			}
			if(true==extMissing)
			{
				continue;
			}
		}

		// The tutorial tells swap-chain support also needs to be checked.
		// Also it tells the check should be done only if it passes swap-chain extension check.
		{
			auto surfaceFom=FsVulkanEnumeratePhysicalDeviceSurfaceFormat(dev,surface);
			auto presentMode=FsVulkanEnumeratePhysicalDeviceSurfacePresentationModesKHR(dev,surface);
			if(0==surfaceFom.size() || 0==presentMode.size())
			{
				continue;
			}
		}

		// Check device prop
		VkPhysicalDeviceProperties prop;
		vkGetPhysicalDeviceProperties(dev,&prop);
		int score=0;
		if(VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU==prop.deviceType)
		{
			score=4;
		}
		else if(VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU==prop.deviceType)
		{
			score=3;
		}
		else if(VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU==prop.deviceType)
		{
			score=2;
		}
		else if(VK_PHYSICAL_DEVICE_TYPE_CPU==prop.deviceType)
		{
			score=1;
		}
		if(VK_NULL_HANDLE==dev || bestScore<score)
		{
			bestDev=dev;
			bestScore=score;
		}
	}
	return bestDev;
}

std::vector <VkQueueFamilyProperties> FsVulkanEnumerateQueueFamily(VkPhysicalDevice dev)
{
	uint32_t numQueueFamily=0;
	vkGetPhysicalDeviceQueueFamilyProperties(dev,&numQueueFamily,nullptr);

	std::vector <VkQueueFamilyProperties> allQueueFamily;
	allQueueFamily.resize(numQueueFamily);
	vkGetPhysicalDeviceQueueFamilyProperties(dev,&numQueueFamily,allQueueFamily.data());

	return allQueueFamily;
}

std::vector <std::string> FsVulkanEnumerateDeviceExtension(VkPhysicalDevice dev)
{
	uint32_t nDevExt;
	vkEnumerateDeviceExtensionProperties(dev,nullptr,&nDevExt,nullptr);
	std::vector <VkExtensionProperties> devExtProp;
	devExtProp.resize(nDevExt);
	vkEnumerateDeviceExtensionProperties(dev,nullptr,&nDevExt,devExtProp.data());

	std::vector <std::string> devExtName;
	for(auto &ext : devExtProp)
	{
		devExtName.push_back(std::string(ext.extensionName));
		printf("Device Extension: %s\n",ext.extensionName);
	}

	return devExtName;
}

std::vector <const char *> FsVulkanSelectDeviceExtension(const std::vector <std::string> &foundExt)
{
	std::vector <const char *> devExtPtr;
	for(auto &ext : foundExt)
	{
		if(ext==std::string(VK_KHR_SWAPCHAIN_EXTENSION_NAME))
		{
			devExtPtr.push_back(ext.data());
		}
	}
	return devExtPtr;
}

uint32_t FsVulkanGetGraphicsQueueFamilyIndex(VkPhysicalDevice dev)
{
	uint32_t idx=0;
	for(auto &queueFamily : FsVulkanEnumerateQueueFamily(dev))
	{
		if(0<queueFamily.queueCount && 0!=(VK_QUEUE_GRAPHICS_BIT & queueFamily.queueFlags))
		{
			return idx;
		}
		++idx;
	}
	return FsVulkan_INVALID_INDEX;
}

uint32_t FsVulkanGetPresentQueueFamilyIndex(VkPhysicalDevice dev,VkSurfaceKHR surface)
{
	uint32_t idx=0;
	for(auto &queueFamily : FsVulkanEnumerateQueueFamily(dev))
	{
		if(0<queueFamily.queueCount)
		{
			VkBool32 presentSupport=false;
			vkGetPhysicalDeviceSurfaceSupportKHR(dev,idx,surface,&presentSupport);
			if(true==presentSupport)
			{
				return idx;
			}
		}
		++idx;
	}
	return FsVulkan_INVALID_INDEX;
}

bool FsVulkanGetQueueFamilyIndex(uint32_t &graphicsQueueIndex,uint32_t &presentQueueIndex,VkPhysicalDevice dev,VkSurfaceKHR surface)
{
	auto allQueueFamily=FsVulkanEnumerateQueueFamily(dev);

	printf("Number of Queue Families:%d\n",(int)allQueueFamily.size());

	graphicsQueueIndex=FsVulkanGetGraphicsQueueFamilyIndex(dev);
	presentQueueIndex=FsVulkanGetPresentQueueFamilyIndex(dev,surface);

	if(FsVulkan_INVALID_INDEX==graphicsQueueIndex)
	{
		printf("Graphics queue family not found.\n");
	}
	else
	{
		printf("Graphics Queue Family Index: %d\n",graphicsQueueIndex);
	}

	if(FsVulkan_INVALID_INDEX==presentQueueIndex)
	{
		printf("Present queue family not found.\n");
	}
	else
	{
		printf("Present Queue Family Index: %d\n",presentQueueIndex);
	}

	if(FsVulkan_INVALID_INDEX!=graphicsQueueIndex && FsVulkan_INVALID_INDEX!=presentQueueIndex)
	{
		return true;
	}
	return false;
}

std::vector <VkSurfaceFormatKHR> FsVulkanEnumeratePhysicalDeviceSurfaceFormat(VkPhysicalDevice dev,VkSurfaceKHR surface)
{
	uint32_t numFormat;
	vkGetPhysicalDeviceSurfaceFormatsKHR(dev,surface,&numFormat,nullptr);
	std::vector <VkSurfaceFormatKHR> surfaceFom;
	surfaceFom.resize(numFormat);
	vkGetPhysicalDeviceSurfaceFormatsKHR(dev,surface,&numFormat,surfaceFom.data());
	return surfaceFom;
}

VkSurfaceFormatKHR FsVulkanSelectSurfaceFormat(const std::vector <VkSurfaceFormatKHR> &allFormat)
{
	bool meaningful=false;
	VkSurfaceFormatKHR pickedFom;
	for(auto &fom : allFormat)
	{
		if(VK_FORMAT_B8G8R8A8_UNORM==fom.format && VK_COLOR_SPACE_SRGB_NONLINEAR_KHR==fom.colorSpace)
		{
			return fom;
		}
		else if(VK_FORMAT_UNDEFINED!=fom.format)
		{
			pickedFom=fom;
			meaningful=true;
		}
	}

	if(true==meaningful)
	{
		return pickedFom;
	}

	return {VK_FORMAT_B8G8R8A8_UNORM,VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
}

std::vector <VkPresentModeKHR> FsVulkanEnumeratePhysicalDeviceSurfacePresentationModesKHR(VkPhysicalDevice dev,VkSurfaceKHR surface)
{
	uint32_t numPresentMode;
	vkGetPhysicalDeviceSurfacePresentModesKHR(dev,surface,&numPresentMode,nullptr);
	std::vector <VkPresentModeKHR> presentMode;
	presentMode.resize(numPresentMode);
	vkGetPhysicalDeviceSurfacePresentModesKHR(dev,surface,&numPresentMode,presentMode.data());
	return presentMode;
}

VkPresentModeKHR FsVulkanSelectPresentMode(const std::vector <VkPresentModeKHR> &allMode)
{
	// According to the tutorial, this mode always is available.
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D FsVulkanSelectSwapExtent(const VkSurfaceCapabilitiesKHR &surfaceCap)
{
	if(surfaceCap.currentExtent.width!=std::numeric_limits<uint32_t>::max())
	{
		return surfaceCap.currentExtent;
	}
	else
	{
		VkExtent2D extent;
		int wid=1200;
		int hei=800;
		if(wid<surfaceCap.minImageExtent.width)
		{
			wid=surfaceCap.minImageExtent.width;
		}
		else if(surfaceCap.maxImageExtent.width<wid)
		{
			wid=surfaceCap.maxImageExtent.width;
		}

		if(hei<surfaceCap.minImageExtent.height)
		{
			hei=surfaceCap.minImageExtent.height;
		}
		else if(surfaceCap.maxImageExtent.height<hei)
		{
			hei=surfaceCap.maxImageExtent.height;
		}

		extent.width=wid;
		extent.height=hei;
		return extent;
	}
}

void FsVulkanFindDepthStencilBufferFormat(VkFormat &fom,bool &hasStencil,VkPhysicalDevice dev)
{
	struct
	{
		VkFormat fom;
		bool stencil;
	} 
	depthBufFormat[]=
	{
		{VK_FORMAT_D32_SFLOAT_S8_UINT,true},
		{VK_FORMAT_D24_UNORM_S8_UINT, true},
		{VK_FORMAT_D32_SFLOAT,        false},
	};

	for(auto f : depthBufFormat)
	{
		VkFormatProperties prop;
		vkGetPhysicalDeviceFormatProperties(dev,f.fom,&prop);
		if(0!=(prop.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT))
		{
			fom=f.fom;
			hasStencil=f.stencil;
			return;
		}
	}

	fom=VK_FORMAT_D32_SFLOAT_S8_UINT; 
	hasStencil=true;  // Just shut up and try it and die.
}

////////////////////////////////////////////////////////////

VkInstance FsVulkanCreateInstance(int useValidationLayer)
{
	FsVulkanContext::GetContext().CreateInstance(useValidationLayer);
	return FsVulkanContext::GetContext().inst;
}

void FsVulkanSetPlatformSpecificSurface(VkSurfaceKHR surface)
{
	FsVulkanContext::GetContext().SetPlatformSpecificSurface(surface);
}

void FsVulkanSelectPhysicalDevice(void)
{
	FsVulkanContext::GetContext().CreatePhysicalDevice();
}

void FsVulkanCreatePhysicalDevice(void)
{
	FsVulkanContext::GetContext().CreatePhysicalDevice();
}

void FsVulkanCreateLogicalDevice(void)
{
	FsVulkanContext::GetContext().CreateLogicalDevice();
}

void FsVulkanCreateSwapChain(void)
{
	FsVulkanContext::GetContext().primarySwapChain.CreateSwapChain();
	FsVulkanContext::GetContext().primarySwapChain.CreateUniformPool();
}

void FsVulkanDestroySwapChain(void)
{
	vkDeviceWaitIdle(FsVulkanContext::GetContext().logicalDevice);
	FsVulkanContext::GetContext().primarySwapChain.DestroyRenderTarget();
	FsVulkanContext::GetContext().primarySwapChain.DestroySwapChain();
	vkDestroySurfaceKHR(FsVulkanContext::GetContext().inst,FsVulkanContext::GetContext().surface,nullptr);
}

void FsVulkanRecreateSwapChain(VkSurfaceKHR surface)
{
	vkDeviceWaitIdle(FsVulkanContext::GetContext().logicalDevice);
	FsVulkanContext::GetContext().SetPlatformSpecificSurface(surface);
	FsVulkanContext::GetContext().primarySwapChain.CreateSwapChain();
	FsVulkanContext::GetContext().primarySwapChain.CreateRenderPass();
	FsVulkanContext::GetContext().primarySwapChain.CreateDepthBuffer();
	FsVulkanContext::GetContext().primarySwapChain.CreateFramebuffer();
	FsVulkanContext::GetContext().primarySwapChain.CreateCommandBuffer();
}

void FsVulkanCreateRenderPass(void)
{
	FsVulkanContext::GetContext().primarySwapChain.CreateRenderPass();
}

void FsVulkanCreateDepthBuffer(void)
{
	FsVulkanContext::GetContext().primarySwapChain.CreateDepthBuffer();
}

void FsVulkanCreateFramebuffer(void)
{
	FsVulkanContext::GetContext().primarySwapChain.CreateFramebuffer();
}

void FsVulkanCreateCommandBuffer(void)
{
	FsVulkanContext::GetContext().primarySwapChain.CreateCommandBuffer();
}

void FsVulkanCreateSemaphore(void)
{
	FsVulkanContext::GetContext().CreateSemaphore();
}

void FsVulkanCreateStockResource(void)
{
	FsVulkanContext::GetContext().CreateStockResoruce();
}

