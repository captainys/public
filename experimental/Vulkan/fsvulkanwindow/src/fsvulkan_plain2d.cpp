#include "fsvulkan.h"
#include "fsvulkan_spirv.h"



void FsVulkanContext::Plain2dRenderer::CmdResetModelTransformation(Frame &frame)
{
	// When I try to submit push_constants as mat3, only the first float is updated.
	// When I submit push_constants as float[9], I get correct values.
	//   -> Because mat3 cannot be used as a push constant.  I don't know mat3 cannot be a uniform in general.

	float modelTfm[6]=
	{
		1,0,
		0,1,
		0,0,
	};
	vkCmdPushConstants(frame.commandBuffer,this->pipelineLayout,VK_SHADER_STAGE_VERTEX_BIT,sizeof(float)*6,sizeof(modelTfm),modelTfm);
}

FsVulkanContext::Plain2dRenderer *FsVulkanContext::RendererPool::CreatePlain2dTriangleRenderer(VkRenderPass renderPass)
{
printf("%s %d\n",__FUNCTION__,__LINE__);
	Plain2dRenderer *renderer;
	renderer=new Plain2dRenderer(this);

	VkPushConstantRange pushConstRange[1]={{}};
	pushConstRange[0].stageFlags=VK_SHADER_STAGE_VERTEX_BIT;
	pushConstRange[0].offset=0;
	pushConstRange[0].size=sizeof(float)*12;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo={};
	pipelineLayoutInfo.sType=VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount=0;
	pipelineLayoutInfo.pushConstantRangeCount=1;
	pipelineLayoutInfo.pPushConstantRanges=pushConstRange;

	if(VK_SUCCESS!=vkCreatePipelineLayout(owner->logicalDevice,&pipelineLayoutInfo,nullptr,&renderer->pipelineLayout)) 
	{
	    printf("Failed to create a pipeline!\n");
	    delete renderer;
	    return nullptr;
	}



////////////////////////////////////////////////////////////



	auto vertShaderModule=owner->CreateShaderModule(plain2d_vert_spv_len,plain2d_vert_spv,"plain2d_vert_spv");
	auto fragShaderModule=owner->CreateShaderModule(plain2d_frag_spv_len,plain2d_frag_spv,"plain2d_frag_spv");


	VkPipelineShaderStageCreateInfo emptyStageInfo={};
	VkPipelineShaderStageCreateInfo stageInfo[2]={emptyStageInfo,emptyStageInfo};

	stageInfo[0].sType=VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stageInfo[0].stage=VK_SHADER_STAGE_VERTEX_BIT;
	stageInfo[0].module=vertShaderModule;
	stageInfo[0].pName="main";

	stageInfo[1].sType=VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stageInfo[1].stage=VK_SHADER_STAGE_FRAGMENT_BIT;
	stageInfo[1].module=fragShaderModule;
	stageInfo[1].pName="main";


	VkPipelineColorBlendAttachmentState colorBlendAttachment={};
	colorBlendAttachment.colorWriteMask=VK_COLOR_COMPONENT_R_BIT|VK_COLOR_COMPONENT_G_BIT|VK_COLOR_COMPONENT_B_BIT|VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable=VK_FALSE;


	VkVertexInputBindingDescription emptyBindingDesc={};
	VkVertexInputBindingDescription bindingDesc[2]={emptyBindingDesc,emptyBindingDesc};
	bindingDesc[0].binding=0;
	bindingDesc[0].stride=sizeof(float)*2; // 2-dimensional
	bindingDesc[0].inputRate=VK_VERTEX_INPUT_RATE_VERTEX;
	bindingDesc[1].binding=1;
	bindingDesc[1].stride=sizeof(float)*4; // RGBA
	bindingDesc[1].inputRate=VK_VERTEX_INPUT_RATE_VERTEX;

	VkVertexInputAttributeDescription emptyAttribDesc={};
	VkVertexInputAttributeDescription attribDesc[2]={emptyAttribDesc,emptyAttribDesc};
	attribDesc[0].binding=0;
	attribDesc[0].location=0;
	attribDesc[0].format=VK_FORMAT_R32G32_SFLOAT;
	attribDesc[0].offset=0;
	attribDesc[1].binding=1;
	attribDesc[1].location=1;
	attribDesc[1].format=VK_FORMAT_R32G32B32A32_SFLOAT;
	attribDesc[1].offset=0;

	renderer->attribBinding.push_back(Renderer::VERTEX2F);
	renderer->attribBinding.push_back(Renderer::COLOR4F);

	VkPipelineVertexInputStateCreateInfo vertexInputInfo={};
	vertexInputInfo.sType=VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount=2;
	vertexInputInfo.pVertexBindingDescriptions=bindingDesc;
	vertexInputInfo.vertexAttributeDescriptionCount=2;
	vertexInputInfo.pVertexAttributeDescriptions=attribDesc;


	VkPipelineInputAssemblyStateCreateInfo inputAssembly={};
	inputAssembly.sType=VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology=VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;                        // <- Primitive Type
	inputAssembly.primitiveRestartEnable=VK_FALSE;


	VkPipelineRasterizationStateCreateInfo rasterizer={};
	rasterizer.sType=VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable=VK_FALSE;
	rasterizer.rasterizerDiscardEnable=VK_FALSE;
	rasterizer.polygonMode=VK_POLYGON_MODE_FILL;                        // <- Maybe only relevant for polygonal primitives?
	rasterizer.lineWidth=1.0f;
	rasterizer.cullMode=0; // VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace=VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable=VK_FALSE;
	rasterizer.depthBiasConstantFactor=0.0f;
	rasterizer.depthBiasClamp=0.0f;
	rasterizer.depthBiasSlopeFactor=0.0f;


	VkPipelineMultisampleStateCreateInfo multisampling={};
	multisampling.sType=VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable=VK_FALSE;
	multisampling.rasterizationSamples=VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading=1.0f;
	multisampling.pSampleMask=nullptr;
	multisampling.alphaToCoverageEnable=VK_FALSE;
	multisampling.alphaToOneEnable=VK_FALSE;


	VkPipelineColorBlendStateCreateInfo colorBlending={};
	colorBlending.sType=VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable=VK_FALSE;
	colorBlending.logicOp=VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount=1;
	colorBlending.pAttachments=&colorBlendAttachment;
	colorBlending.blendConstants[0]=0.0f;
	colorBlending.blendConstants[1]=0.0f;
	colorBlending.blendConstants[2]=0.0f;
	colorBlending.blendConstants[3]=0.0f;


	VkViewport viewport={};
	viewport.x=0.0f;
	viewport.y=0.0f;
	viewport.width=100.0f;
	viewport.height=100.0f;
	viewport.minDepth=0.0f;
	viewport.maxDepth=1.0f;

	VkRect2D scissor={};
	scissor.offset={0,0};
	scissor.extent.width=100.0f;
	scissor.extent.height=100.0f;

	// Question: Will this viewport be used as a default viewport if I make it a dynamic viewport state?
	//           Static viewport state is pretty much useless by the way.
	VkPipelineViewportStateCreateInfo viewportState={};
	viewportState.sType=VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount=1;
	viewportState.pViewports=&viewport;
	viewportState.scissorCount=1;
	viewportState.pScissors=&scissor;


	VkDynamicState dynamicState[]={VK_DYNAMIC_STATE_VIEWPORT,VK_DYNAMIC_STATE_SCISSOR,VK_DYNAMIC_STATE_LINE_WIDTH};
	const int nDynamicState=sizeof(dynamicState)/sizeof(dynamicState[0]);

	VkPipelineDynamicStateCreateInfo dynamicStateInfo={};
	dynamicStateInfo.sType=VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicStateInfo.dynamicStateCount=nDynamicState;
	dynamicStateInfo.pDynamicStates=dynamicState;


	VkPipelineDepthStencilStateCreateInfo depthStencil={};
	depthStencil.sType=VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable=VK_FALSE;
	depthStencil.depthWriteEnable=VK_FALSE;
	depthStencil.depthCompareOp=VK_COMPARE_OP_ALWAYS;
	depthStencil.depthBoundsTestEnable=VK_FALSE;
	depthStencil.stencilTestEnable=VK_FALSE;


	VkGraphicsPipelineCreateInfo pipelineInfo={};
	pipelineInfo.sType=VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount=2;
	pipelineInfo.pStages=stageInfo;
	pipelineInfo.pVertexInputState=&vertexInputInfo;
	pipelineInfo.pInputAssemblyState=&inputAssembly;
	pipelineInfo.pViewportState=&viewportState;
	pipelineInfo.pRasterizationState=&rasterizer;
	pipelineInfo.pMultisampleState=&multisampling;
	pipelineInfo.pDepthStencilState=&depthStencil;
	pipelineInfo.pColorBlendState=&colorBlending;
	pipelineInfo.pDynamicState=&dynamicStateInfo;
	pipelineInfo.layout=renderer->pipelineLayout;
	pipelineInfo.renderPass=renderPass;
	pipelineInfo.subpass=0;
	pipelineInfo.basePipelineHandle=VK_NULL_HANDLE;

printf("%s %d\n",__FUNCTION__,__LINE__);
	if(VK_SUCCESS!=vkCreateGraphicsPipelines(owner->logicalDevice,VK_NULL_HANDLE,1,&pipelineInfo,nullptr,&renderer->triangleListPipeline))
	{
		printf("Failed to create a pipeline!\n");
	}

	inputAssembly.topology=VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
	if(VK_SUCCESS!=vkCreateGraphicsPipelines(owner->logicalDevice,VK_NULL_HANDLE,1,&pipelineInfo,nullptr,&renderer->pointListPipeline))
	{
		printf("Failed to create a pipeline!\n");
	}

	inputAssembly.topology=VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
	if(VK_SUCCESS!=vkCreateGraphicsPipelines(owner->logicalDevice,VK_NULL_HANDLE,1,&pipelineInfo,nullptr,&renderer->lineListPipeline))
	{
		printf("Failed to create a pipeline!\n");
	}
	printf("Pipeline Created!\n");



	vkDestroyShaderModule(owner->logicalDevice,fragShaderModule,nullptr);
	vkDestroyShaderModule(owner->logicalDevice,vertShaderModule,nullptr);

	createdRenderer.push_back(renderer);

	return renderer;
}

void FsVulkanContext::Plain2dRenderer::CmdUsePixelCoordTopLeftAsOrigin(Frame &frame)
{
	// (0,0) mapped to (-1,-1)
	// (wid,hei) mapped to (1,1)
	// 2.0 scaled up to wid in X direction and hei in Y direction.

	auto ext=frame.imageExtent;
	const float scaleX=2.0f/(float)ext.width;
	const float scaleY=2.0f/(float)ext.height;
	const float transX=-1.0f,transY=-1.0f;

	const float tfm[6]=
	{
		scaleX, 0,
		0     , scaleY,
		transX, transY
	};
	CmdSetWindowTransformation(frame,tfm);
}
void FsVulkanContext::Plain2dRenderer::CmdUseNormalizedCoordTopLeftAsOrigin(Frame &frame)
{
	const float t[6]=
	{
		1,0,
		0,1,
		0,0
	};
	CmdSetWindowTransformation(frame,t);
}
void FsVulkanContext::Plain2dRenderer::CmdSetWindowTransformation(Frame &frame,const float t[6])
{
	vkCmdPushConstants(frame.commandBuffer,this->pipelineLayout,VK_SHADER_STAGE_VERTEX_BIT,0              ,sizeof(float)*6,t);
}
void FsVulkanContext::Plain2dRenderer::CmdSetModelTransformation(Frame &frame,const float t[6])
{
	vkCmdPushConstants(frame.commandBuffer,this->pipelineLayout,VK_SHADER_STAGE_VERTEX_BIT,sizeof(float)*6,sizeof(float)*6,t);
}
