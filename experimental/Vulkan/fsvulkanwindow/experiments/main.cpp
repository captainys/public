#include <math.h>
#include <chrono>

#include "ysglmath.h"

#include "fssimplewindow.h"
#include "fsvulkan.h"





int main(void)
{
	FsOpenWindow(0,0,800,600,1);

	auto &vulkan=FsVulkanGetContext();

	auto renderer=vulkan.primaryRendererPool.CreateExperimentQuad2dRenderer(vulkan.primarySwapChain.renderPass);



	int ctr=0;
	double pitch=0.0,heading=0.0;
	for(;;)
	{
		printf("%d\n",ctr++);

		FsPollDevice();
		auto key=FsInkey();
		if(FSKEY_ESC==key)
		{
			break;
		}
		int wid,hei;
		FsGetWindowSize(wid,hei);



		auto &frame=vulkan.primarySwapChain.ReadyFrame();



		frame.BeginCommandBuffer();


		auto renderPassInfo=vulkan.primarySwapChain.MakeRenderPassBeginInfo(frame);
		renderPassInfo.clearColor[0]={0.7f,0.7f,1.0f,0.0f};
		vkCmdBeginRenderPass(frame.commandBuffer,&renderPassInfo.beginInfo,VK_SUBPASS_CONTENTS_INLINE);




		const float quad1[]=
		{
			10.0f,10.0f,0,1,
			100.0f,10.0f,0,1,
			100.0f,100.0f,0,1,
			10.0f,100.0f,0,1,
		};
		const float color1[]=
		{
			1,0,0,1,
			0,1,0,1,
			1,0,0,1,
			0,0,1,1,
		};

		const float quad2[]=
		{
			110.0f, 10.0f,0,1,
			200.0f, 10.0f,0,1,
			200.0f,100.0f,0,1,
			110.0f,100.0f,0,1,
		};
		const float color2[]=
		{
			1,0,0,1,
			0,1,0,1,
			1,0,0,1,
			0,0,1,1,
		};



		vkCmdBindPipeline(frame.commandBuffer,VK_PIPELINE_BIND_POINT_GRAPHICS,renderer->triangleListPipeline);
		renderer->CmdUsePixelCoordTopLeftAsOrigin(frame);
		renderer->CmdResetModelTransformation(frame);



		for(int i=0; i<16; ++i)
		{
			renderer->statePtr->vertex[i]=quad1[i];
			renderer->statePtr->color[i]=color1[i];
		}
		VkDescriptorSet uniformBinder[1]={renderer->CreateCurrentStateUniformBinder(frame)};
		vkCmdBindDescriptorSets(frame.commandBuffer,VK_PIPELINE_BIND_POINT_GRAPHICS,renderer->pipelineLayout,0,1,uniformBinder,0,nullptr);
		vkCmdDraw(frame.commandBuffer,6,1,0,0);
printf("%llx\n",uniformBinder[0]);

		for(int i=0; i<16; ++i)
		{
			renderer->statePtr->vertex[i]=quad2[i];
			renderer->statePtr->color[i]=color2[i];
		}
		uniformBinder[0]=renderer->CreateCurrentStateUniformBinder(frame);
		vkCmdBindDescriptorSets(frame.commandBuffer,VK_PIPELINE_BIND_POINT_GRAPHICS,renderer->pipelineLayout,0,1,uniformBinder,0,nullptr);
		vkCmdDraw(frame.commandBuffer,6,1,0,0);
printf("%llx\n",uniformBinder[0]);





		vkCmdEndRenderPass(frame.commandBuffer);

		vkEndCommandBuffer(frame.commandBuffer);

		vulkan.SubmitCommandBuffer(frame.commandBuffer);
		vulkan.ShowFrame(frame);

		double PI=3.14159265359;
		double t=(double)FsPassedTime()/1000.0;


		if(FsGetKeyState(FSKEY_SPACE))
		{
			heading+=t*PI*1.5;
		}
	}

	vkDeviceWaitIdle(vulkan.logicalDevice);
	vulkan.CleanUp();

	return 0;
}
