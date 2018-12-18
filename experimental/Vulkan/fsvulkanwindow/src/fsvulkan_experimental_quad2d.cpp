#include "fsvulkan.h"
#include "fsvulkan_spirv.h"





void FsVulkanContext::ExperimentQuad2dRenderer::CleanUp(void)
{
	Plain2dRenderer::CleanUp();
}

FsVulkanContext::ExperimentQuad2dRenderer *FsVulkanContext::RendererPool::CreateExperimentQuad2dRenderer(VkRenderPass renderPass)
{
	typedef ExperimentQuad2dRenderer RENDERERCLASS;
	auto renderer=new RENDERERCLASS(this);

	// Uniforms >>
	std::vector <VkDescriptorSetLayout> uniformLayout;
	// Binding 0 Projection matrix
	{
		VkDescriptorSetLayoutBinding uniformBinding[1]={{}};
		uniformBinding[0].binding=0;
		uniformBinding[0].descriptorCount=1;
		uniformBinding[0].descriptorType=VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uniformBinding[0].pImmutableSamplers=nullptr;
		uniformBinding[0].stageFlags=VK_SHADER_STAGE_VERTEX_BIT|VK_SHADER_STAGE_FRAGMENT_BIT;
		long long int length=sizeof(RENDERERCLASS::State);

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
	}
	// Uniforms <<

	// Push Constants (a kind of uniform) >>
	VkPushConstantRange pushConstRange[1]={{}};
	pushConstRange[0].stageFlags=VK_SHADER_STAGE_VERTEX_BIT;
	pushConstRange[0].offset=0;
	pushConstRange[0].size=sizeof(float)*12;
	// Push Constants (a kind of uniform) <<

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



	auto vertShaderModule=owner->CreateShaderModule(experimentQuad2d_vert_spv_len,experimentQuad2d_vert_spv,"experimentQuad2d_vert_spv");
	auto fragShaderModule=owner->CreateShaderModule(experimentQuad2d_frag_spv_len,experimentQuad2d_frag_spv,"experimentQuad2d_frag_spv");


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


	// Vertex attributes >>
	VkPipelineVertexInputStateCreateInfo vertexInputInfo={};
	vertexInputInfo.sType=VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount=0;
	vertexInputInfo.pVertexBindingDescriptions=nullptr;
	vertexInputInfo.vertexAttributeDescriptionCount=0;
	vertexInputInfo.pVertexAttributeDescriptions=nullptr;
	// Vertex attributes <<


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
	printf("Pipeline Created!\n");



	// Create Uniform buffers >>
	VkBufferCreateInfo uniformBufInfo={};
	uniformBufInfo.sType=VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	uniformBufInfo.size=sizeof(RENDERERCLASS::State);
	uniformBufInfo.usage=VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;   // <- It says "uniform"
	uniformBufInfo.sharingMode=VK_SHARING_MODE_EXCLUSIVE;
	vkCreateBuffer(owner->logicalDevice,&uniformBufInfo,nullptr,&renderer->stateUniform.buf);

	renderer->stateUniform.mem=owner->AllocMemoryForBuffer(
		renderer->stateUniform.buf,VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	if(VK_NULL_HANDLE!=renderer->stateUniform.mem)
	{
		vkBindBufferMemory(owner->logicalDevice,renderer->stateUniform.buf,renderer->stateUniform.mem,0);
		renderer->stateUniform.length=sizeof(RENDERERCLASS::State);

		void *data;
		vkMapMemory(owner->logicalDevice,renderer->stateUniform.mem,0,sizeof(RENDERERCLASS::State),0,&data);

		renderer->statePtr=(RENDERERCLASS::State *)data;
	}
	// Create Uniform buffers <<



	vkDestroyShaderModule(owner->logicalDevice,fragShaderModule,nullptr);
	vkDestroyShaderModule(owner->logicalDevice,vertShaderModule,nullptr);

	createdRenderer.push_back(renderer);

	return renderer;
}

VkDescriptorSet FsVulkanContext::ExperimentQuad2dRenderer::CreateCurrentStateUniformBinder(Frame &frame) const
{
	return frame.CreatePerFrameUniformBinder(*this,uniformBinding[0].binding,stateUniform);
}
