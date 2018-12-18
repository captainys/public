#include "fsvulkan.h"
#include "fsvulkan_spirv.h"




FsVulkanContext::PointSprite3dRenderer::PointSprite3dRenderer(RendererPool *owner) : Renderer3dBase(owner)
{
	currentState={};
}

VkDescriptorSet FsVulkanContext::PointSprite3dRenderer::CreateCurrentStateUniformBinder(Frame &frame) const
{
	return frame.CreatePerFrameUniformBinder(*this,uniformBinding[0].binding,sizeof(currentState),&currentState);
}

FsVulkanContext::PointSprite3dRenderer *FsVulkanContext::RendererPool::CreatePointSprite3dRenderer(VkRenderPass renderPass)
{
printf("%s %d\n",__FUNCTION__,__LINE__);
	PointSprite3dRenderer *renderer;
	renderer=new PointSprite3dRenderer(this);

	// Push Constants >>
	VkPushConstantRange pushConstRange[1]={{}};
	pushConstRange[0].stageFlags=VK_SHADER_STAGE_VERTEX_BIT;
	pushConstRange[0].offset=0;
	pushConstRange[0].size=sizeof(float)*16;
	// Push Constants <<

	// Uniforms >>
	std::vector <VkDescriptorSetLayout> uniformLayout;
	VkDescriptorSetLayoutBinding uniformBinding[1]={{}};
	uniformBinding[0].binding=0;
	uniformBinding[0].descriptorCount=1;
	uniformBinding[0].descriptorType=VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uniformBinding[0].pImmutableSamplers=nullptr;
	uniformBinding[0].stageFlags=VK_SHADER_STAGE_VERTEX_BIT|VK_SHADER_STAGE_FRAGMENT_BIT;
	long long int length=sizeof(PointSprite3dRenderer::State);

	VkDescriptorSetLayoutCreateInfo descriptorLayoutInfo={};
	descriptorLayoutInfo.sType=VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorLayoutInfo.bindingCount=sizeof(uniformBinding)/sizeof(uniformBinding[0]);
	descriptorLayoutInfo.pBindings=uniformBinding;

	VkDescriptorSetLayout descSetLayout;
	if(VK_SUCCESS==vkCreateDescriptorSetLayout(owner->logicalDevice,&descriptorLayoutInfo,nullptr,&descSetLayout))
	{
		renderer->AddUniformBinding(uniformBinding[0].binding,descSetLayout,length);

		// Not to confuse, it is a good idea to make binding 0 corresponds to uniformLayout[0].
		uniformLayout.push_back(descSetLayout);
	}
	else
	{
		printf("Failed to create a layout for sampler2d.\n");
	}

	// Binding 1 Sampler2d
	{
		VkDescriptorSetLayoutBinding uniformBinding[1]={{}};
		uniformBinding[0].binding=1;
		uniformBinding[0].descriptorCount=1;
		uniformBinding[0].descriptorType=VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		uniformBinding[0].pImmutableSamplers=nullptr;
		uniformBinding[0].stageFlags=VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutCreateInfo descriptorLayoutInfo={};
		descriptorLayoutInfo.sType=VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorLayoutInfo.bindingCount=sizeof(uniformBinding)/sizeof(uniformBinding[0]);
		descriptorLayoutInfo.pBindings=uniformBinding;

		VkDescriptorSetLayout descSetLayout;
		if(VK_SUCCESS==vkCreateDescriptorSetLayout(owner->logicalDevice,&descriptorLayoutInfo,nullptr,&descSetLayout))
		{
			renderer->AddUniformBinding(uniformBinding[0].binding,descSetLayout,0);

			// Not to confuse, it is a good idea to make binding 0 corresponds to uniformLayout[0].
			uniformLayout.push_back(descSetLayout);
		}
	}
	// Uniforms <<

	VkPipelineLayoutCreateInfo pipelineLayoutInfo={};
	pipelineLayoutInfo.sType=VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount=uniformLayout.size();
	pipelineLayoutInfo.pSetLayouts=uniformLayout.data();
	pipelineLayoutInfo.pushConstantRangeCount=1;
	pipelineLayoutInfo.pPushConstantRanges=pushConstRange;

	if(VK_SUCCESS!=vkCreatePipelineLayout(owner->logicalDevice,&pipelineLayoutInfo,nullptr,&renderer->pipelineLayout)) 
	{
	    printf("Failed to create a pipeline!\n");
	    delete renderer;
	    return nullptr;
	}



////////////////////////////////////////////////////////////



	auto vertShaderModule=owner->CreateShaderModule(pointSprite3d_vert_spv_len,pointSprite3d_vert_spv,"pointSprite3d_vert_spv");
	auto fragShaderModule=owner->CreateShaderModule(pointSprite3d_frag_spv_len,pointSprite3d_frag_spv,"pointSprite3d_frag_spv");


	VkPipelineShaderStageCreateInfo stageInfo[2]={{},{}};

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
	colorBlendAttachment.blendEnable=VK_TRUE;
	colorBlendAttachment.srcColorBlendFactor=VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlendAttachment.dstColorBlendFactor=VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorBlendAttachment.alphaBlendOp=VK_BLEND_OP_ADD;


	VkVertexInputBindingDescription bindingDesc[]={{},{},{},{}};
	bindingDesc[0].binding=0;
	bindingDesc[0].stride=sizeof(float)*3; // 3-dimensional
	bindingDesc[0].inputRate=VK_VERTEX_INPUT_RATE_VERTEX;
	bindingDesc[1].binding=1;
	bindingDesc[1].stride=sizeof(float)*4; // RGBA
	bindingDesc[1].inputRate=VK_VERTEX_INPUT_RATE_VERTEX;
	bindingDesc[2].binding=2;
	bindingDesc[2].stride=sizeof(float)*2; // 2-dimensional Tex Coord
	bindingDesc[2].inputRate=VK_VERTEX_INPUT_RATE_VERTEX;
	bindingDesc[3].binding=3;
	bindingDesc[3].stride=sizeof(float);   // Point Size
	bindingDesc[3].inputRate=VK_VERTEX_INPUT_RATE_VERTEX;


	VkVertexInputAttributeDescription attribDesc[]={{},{},{},{}};
	attribDesc[0].binding=0;
	attribDesc[0].location=0;
	attribDesc[0].format=VK_FORMAT_R32G32B32_SFLOAT;
	attribDesc[0].offset=0;
	attribDesc[1].binding=1;
	attribDesc[1].location=1;
	attribDesc[1].format=VK_FORMAT_R32G32B32A32_SFLOAT;
	attribDesc[1].offset=0;
	attribDesc[2].binding=2;
	attribDesc[2].location=2;
	attribDesc[2].format=VK_FORMAT_R32G32_SFLOAT;
	attribDesc[2].offset=0;
	attribDesc[3].binding=3;
	attribDesc[3].location=3;
	attribDesc[3].format=VK_FORMAT_R32_SFLOAT;
	attribDesc[3].offset=0;
	renderer->attribBinding.push_back(Renderer::VERTEX3F);
	renderer->attribBinding.push_back(Renderer::COLOR4F);
	renderer->attribBinding.push_back(Renderer::TEXCOORD2F);
	renderer->attribBinding.push_back(Renderer::POINTSIZE1F);

	VkPipelineVertexInputStateCreateInfo vertexInputInfo={};
	vertexInputInfo.sType=VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount=4;
	vertexInputInfo.pVertexBindingDescriptions=bindingDesc;
	vertexInputInfo.vertexAttributeDescriptionCount=4;
	vertexInputInfo.pVertexAttributeDescriptions=attribDesc;


	VkPipelineInputAssemblyStateCreateInfo inputAssembly={};
	inputAssembly.sType=VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology=VK_PRIMITIVE_TOPOLOGY_POINT_LIST;                        // <- Primitive Type
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
	depthStencil.depthTestEnable=VK_TRUE;
	depthStencil.depthWriteEnable=VK_TRUE;
	depthStencil.depthCompareOp=VK_COMPARE_OP_LESS_OR_EQUAL;
	depthStencil.depthBoundsTestEnable=VK_TRUE;
	depthStencil.minDepthBounds=0;
	depthStencil.maxDepthBounds=1;
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
	if(VK_SUCCESS!=vkCreateGraphicsPipelines(owner->logicalDevice,VK_NULL_HANDLE,1,&pipelineInfo,nullptr,&renderer->pointListPipeline))
	{
		printf("Failed to create a pipeline!\n");
	}
	printf("Pipeline Created!\n");



	vkDestroyShaderModule(owner->logicalDevice,fragShaderModule,nullptr);
	vkDestroyShaderModule(owner->logicalDevice,vertShaderModule,nullptr);

	createdRenderer.push_back(renderer);

	return renderer;
}
