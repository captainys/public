#include <math.h>
#include <chrono>

#include "ysglmath.h"

#include "fssimplewindow.h"
#include "fsvulkan.h"





int main(void)
{
	FsOpenWindow(0,0,800,600,1);

	auto &vulkan=FsVulkanGetContext();

	auto pointSprite3dRenderer=vulkan.primaryRendererPool.CreatePointSprite3dRenderer(vulkan.primarySwapChain.renderPass);



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





		float modelView[16];
		const double aspect=(double)wid/(double)hei;

		YsGLMakePerspectivefv(pointSprite3dRenderer->currentState.projection,YSGLPI/6.0,aspect,1.0,50.0);

		YsGLMakeIdentityfv(modelView);
		YsGLMultMatrixTranslationfv(modelView,0.0f,0.0f,-5.0f);
		YsGLMultMatrixRotationZYfv(modelView,pitch);
		YsGLMultMatrixRotationXZfv(modelView,heading);





		const float vtx[]=
		{
			-1,-1,-1,  1,-1,-1, -1, 1,-1,  1, 1,-1,
			-1,-1, 1,  1,-1, 1, -1, 1, 1,  1, 1, 1,
		};
		const float col[]=
		{
			0.5f,0.5f,0.5f,1,  0,0,1,1, 0,1,0,1, 0,1,1,1,
			1,0,0,1,           1,0,1,1, 1,1,0,1, 1,1,1,1,
		};
		const float texCoord[]=
		{
			0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 
		};
		const float pointSize[]=
		{
			1,1,1,1,1,1,1,1
		};
		auto vtxBuf=frame.CreatePerFrameVertexBuffer(sizeof(vtx),vtx);
		auto colBuf=frame.CreatePerFrameVertexBuffer(sizeof(col),col);
		auto texCoordBuf=frame.CreatePerFrameVertexBuffer(sizeof(texCoord),texCoord);
		auto pointSizeBuf=frame.CreatePerFrameVertexBuffer(sizeof(pointSize),pointSize);

		vkCmdBindPipeline(frame.commandBuffer,VK_PIPELINE_BIND_POINT_GRAPHICS,pointSprite3dRenderer->pointListPipeline);





		pointSprite3dRenderer->currentState.viewportWid=(float)wid;
		pointSprite3dRenderer->currentState.viewportHei=(float)hei;
		pointSprite3dRenderer->currentState.usePointSizeInPixel=0;
		pointSprite3dRenderer->currentState.usePointSizeIn3D=1;
		pointSprite3dRenderer->currentState.texCoordRange=0.5;
		pointSprite3dRenderer->currentState.useTexture=1;
		pointSprite3dRenderer->currentState.usePointClippingCircle=1;

		VkDescriptorSet uniformBinder[]=
		{
			pointSprite3dRenderer->CreateCurrentStateUniformBinder(frame)
		};
		VkDescriptorSet texBinder[]=
		{
			frame.CreatePerFrameSamplerBinder(*pointSprite3dRenderer,1,vulkan.GetStockCheckerTexture2d())
		};
		vkCmdBindDescriptorSets(frame.commandBuffer,VK_PIPELINE_BIND_POINT_GRAPHICS,pointSprite3dRenderer->pipelineLayout,0,1,uniformBinder,0,nullptr);
		vkCmdBindDescriptorSets(frame.commandBuffer,VK_PIPELINE_BIND_POINT_GRAPHICS,pointSprite3dRenderer->pipelineLayout,1,1,texBinder,0,nullptr);

		VkBuffer attribBinder[]={vtxBuf.buf,colBuf.buf,texCoordBuf.buf,pointSizeBuf.buf};
		VkDeviceSize attribBindOffset[]={0,0,0,0};
		vkCmdBindVertexBuffers(frame.commandBuffer,0,sizeof(attribBinder)/sizeof(attribBinder[0]),attribBinder,attribBindOffset);

		pointSprite3dRenderer->CmdSetModelView(frame,modelView);
		vkCmdDraw(frame.commandBuffer,8,1,0,0);





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
