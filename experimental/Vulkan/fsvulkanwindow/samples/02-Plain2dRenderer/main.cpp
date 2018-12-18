#include "fssimplewindow.h"
#include "fsvulkan.h"





int main(void)
{
	FsOpenWindow(0,0,800,600,1);

	auto &vulkan=FsVulkanGetContext();
	auto plain2dRenderer=vulkan.primaryRendererPool.CreatePlain2dTriangleRenderer(vulkan.primarySwapChain.renderPass);

	int ctr=0;
	for(;;)
	{
		printf("%d\n",ctr++);

		FsPollDevice();
		auto key=FsInkey();
		if(FSKEY_ESC==key)
		{
			break;
		}



		auto &frame=vulkan.primarySwapChain.ReadyFrame();
		frame.BeginCommandBuffer();

		// 
		float triVtx2d[]=
		{
			100,100,
			700,100,
			100,500,
		};
		float triColor[]=
		{
			1.0f,0.0f,0.0f,1.0f,
			0.0f,1.0f,0.0f,1.0f,
			0.0f,1.0f,1.0f,1.0f,
		};
		float lineVtx2d[]=
		{
			700,100, 700,500,
			700,500, 100,500
		};
		float lineColor[]=
		{
			1,1,1,1, 1,1,1,1,
			1,1,1,1, 1,1,1,1,
		};


		auto renderPassInfo=vulkan.primarySwapChain.MakeRenderPassBeginInfo(frame);
		renderPassInfo.clearColor[0]={0,0,1,0};
		vkCmdBeginRenderPass(frame.commandBuffer,&renderPassInfo.beginInfo,VK_SUBPASS_CONTENTS_INLINE);



		plain2dRenderer->CmdUsePixelCoordTopLeftAsOrigin(frame);
		plain2dRenderer->CmdResetModelTransformation(frame);
		plain2dRenderer->CmdUsePixelCoordTopLeftAsOrigin(frame);

		auto triVtxBuf=frame.CreatePerFrameVertexBuffer(sizeof(triVtx2d),triVtx2d);
		auto triColBuf=frame.CreatePerFrameVertexBuffer(sizeof(triColor),triColor);

		auto lineVtxBuf=frame.CreatePerFrameVertexBuffer(sizeof(lineVtx2d),lineVtx2d);
		auto lineColBuf=frame.CreatePerFrameVertexBuffer(sizeof(lineColor),lineColor);

		vkCmdBindPipeline(frame.commandBuffer,VK_PIPELINE_BIND_POINT_GRAPHICS,plain2dRenderer->triangleListPipeline);

		VkBuffer toBind[2]={triVtxBuf.buf,triColBuf.buf};
		VkDeviceSize toBindOffset[2]={0,0};
		vkCmdBindVertexBuffers(frame.commandBuffer,0,2,toBind,toBindOffset);
		vkCmdDraw(frame.commandBuffer,3,1,0,0);


		vkCmdBindPipeline(frame.commandBuffer,VK_PIPELINE_BIND_POINT_GRAPHICS,plain2dRenderer->lineListPipeline);

		VkBuffer lineBind[2]={lineVtxBuf.buf,lineColBuf.buf};
		VkDeviceSize lineBindOffset[2]={0,0};
		vkCmdBindVertexBuffers(frame.commandBuffer,0,2,lineBind,lineBindOffset);
		vkCmdDraw(frame.commandBuffer,4,1,0,0);


		vkCmdEndRenderPass(frame.commandBuffer);
		vkEndCommandBuffer(frame.commandBuffer);

		vulkan.SubmitCommandBuffer(frame.commandBuffer);
		vulkan.ShowFrame(frame);
	}

	vkDeviceWaitIdle(vulkan.logicalDevice);
	vulkan.CleanUp();

	return 0;
}
