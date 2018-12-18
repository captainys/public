#include <chrono>

#include "fssimplewindow.h"
#include "fsvulkan.h"




const unsigned char bmp1[16*16]=
{
	0,1,2,3,4,5,6,7, 0,1,0,1,0,1,0,1, 
	1,2,3,4,5,6,7,0, 1,0,1,0,1,0,1,0, 
	2,3,4,5,6,7,0,1, 0,1,0,1,0,1,0,1, 
	3,4,5,6,7,0,1,2, 1,0,1,0,1,0,1,0, 
	4,5,6,7,0,1,2,3, 0,1,0,1,0,1,0,1, 
	5,6,7,0,1,2,3,4, 1,0,1,0,1,0,1,0, 
	6,7,0,1,2,3,4,5, 0,1,0,1,0,1,0,1, 
	7,0,1,2,3,4,5,6, 1,0,1,0,1,0,1,0, 

	0,1,0,1,0,1,0,1, 0,1,2,3,4,5,6,7,
	1,0,1,0,1,0,1,0, 1,2,3,4,5,6,7,0,
	0,1,0,1,0,1,0,1, 2,3,4,5,6,7,0,1,
	1,0,1,0,1,0,1,0, 3,4,5,6,7,0,1,2,
	0,1,0,1,0,1,0,1, 4,5,6,7,0,1,2,3,
	1,0,1,0,1,0,1,0, 5,6,7,0,1,2,3,4,
	0,1,0,1,0,1,0,1, 6,7,0,1,2,3,4,5,
	1,0,1,0,1,0,1,0, 7,0,1,2,3,4,5,6,
};

const unsigned char bmp2[16*16]=
{
	2,2,2,2,2,2,2,2, 4,4,4,4,4,4,4,4,
	7,2,2,2,2,2,2,2, 4,4,4,4,4,4,4,5,
	7,7,2,2,2,2,2,2, 4,4,4,4,4,4,5,5,
	7,7,7,2,2,2,2,2, 4,4,4,4,4,5,5,5,
	7,7,7,7,2,2,2,2, 4,4,4,4,5,5,5,5,
	7,7,7,7,7,2,2,2, 4,4,4,5,5,5,5,5,
	7,7,7,7,7,7,7,2, 4,4,5,5,5,5,5,5,
	7,7,7,7,7,7,7,7, 4,5,5,5,5,5,5,5,

	6,6,6,6,6,6,6,6, 2,4,4,4,4,4,4,4,
	6,6,6,6,6,6,6,7, 2,2,4,4,4,4,4,4,
	6,6,6,6,6,7,7,7, 2,2,2,4,4,4,4,4,
	6,6,6,6,7,7,7,7, 2,2,2,2,4,4,4,4,
	6,6,6,7,7,7,7,7, 2,2,2,2,2,4,4,4,
	6,6,7,7,7,7,7,7, 2,2,2,2,2,2,4,4,
	6,7,7,7,7,7,7,7, 2,2,2,2,2,2,2,4,
	7,7,7,7,7,7,7,7, 2,2,2,2,2,2,2,2,
};

std::vector <unsigned char> EightColorToRGBA(int n,const unsigned char eightCol[])
{
	std::vector <unsigned char> rgba;
	rgba.resize(n*4);
	for(int i=0; i<n; ++i)
	{
		auto c=eightCol[i];
		int b=255*(c&1);
		int r=255*((c&2)>>1);
		int g=255*((c&4)>>2);
		rgba[i*4  ]=r;
		rgba[i*4+1]=g;
		rgba[i*4+2]=b;
		rgba[i*4+3]=255;
	}
	return rgba;
}



int main(void)
{
	FsOpenWindow(0,0,800,600,1);

	auto &vulkan=FsVulkanGetContext();
	auto texture2dRenderer=vulkan.primaryRendererPool.CreateTexture2dTriangleRenderer(vulkan.primarySwapChain.renderPass);

	auto rgba1=EightColorToRGBA(256,::bmp1);
	auto tex2d1=vulkan.CreateTexture2d(16,16,rgba1.data());

	auto rgba2=EightColorToRGBA(256,::bmp2);
	auto tex2d2=vulkan.CreateTexture2d(16,16,rgba2.data());

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



		auto renderPassInfo=vulkan.primarySwapChain.MakeRenderPassBeginInfo(frame);
		renderPassInfo.clearColor[0]={0.7f,0.7f,1.0f,0.0f};
		vkCmdBeginRenderPass(frame.commandBuffer,&renderPassInfo.beginInfo,VK_SUBPASS_CONTENTS_INLINE);





		float texTriVtx2d[]=
		{
			400,300,
			600,300,
			400,500,

			400,500,
			600,500,
			600,300,
		};
		float texTriColor[]=
		{
			1.0f,1.0f,1.0f,1.0f,
			1.0f,1.0f,1.0f,1.0f,
			1.0f,1.0f,1.0f,1.0f,

			1.0f,1.0f,1.0f,1.0f,
			1.0f,1.0f,1.0f,1.0f,
			1.0f,1.0f,1.0f,1.0f,
		};
		float texTriTexCoord[]=
		{
			0,0,
			1,0,
			0,1,

			0,1,
			1,1,
			1,0,
		};
		auto texTriVtxBuf=frame.CreatePerFrameVertexBuffer(sizeof(texTriVtx2d),texTriVtx2d);
		auto texTriColBuf=frame.CreatePerFrameVertexBuffer(sizeof(texTriColor),texTriColor);
		auto texTriTexCoordBuf=frame.CreatePerFrameVertexBuffer(sizeof(texTriTexCoord),texTriTexCoord);

		texture2dRenderer->CmdResetModelTransformation(frame);
		texture2dRenderer->CmdUsePixelCoordTopLeftAsOrigin(frame);

		vkCmdBindPipeline(frame.commandBuffer,VK_PIPELINE_BIND_POINT_GRAPHICS,texture2dRenderer->triangleListPipeline);

		VkBuffer toBindTex[3]={texTriVtxBuf.buf,texTriColBuf.buf,texTriTexCoordBuf.buf};
		VkDeviceSize toBindTexOffset[3]={0,0,0};

		vkCmdBindVertexBuffers(frame.commandBuffer,0,3,toBindTex,toBindTexOffset);

		auto tex1binder=frame.CreatePerFrameSamplerBinder(*texture2dRenderer,0,tex2d1);
		vkCmdBindDescriptorSets(frame.commandBuffer,VK_PIPELINE_BIND_POINT_GRAPHICS,texture2dRenderer->pipelineLayout,0,1,&tex1binder,0,nullptr);

		texture2dRenderer->CmdUsePixelCoordTopLeftAsOrigin(frame);

		vkCmdDraw(frame.commandBuffer,6,1,0,0);



		float texTriVtx2d2[]=
		{
			100,100,
			300,100,
			100,300,

			100,300,
			300,300,
			300,100,
		};
		auto texTriVtxBuf2=frame.CreatePerFrameVertexBuffer(sizeof(texTriVtx2d2),texTriVtx2d2);
		VkBuffer toBindTex2[3]={texTriVtxBuf2.buf,texTriColBuf.buf,texTriTexCoordBuf.buf};

		vkCmdBindVertexBuffers(frame.commandBuffer,0,3,toBindTex2,toBindTexOffset);

		auto tex2binder=frame.CreatePerFrameSamplerBinder(*texture2dRenderer,0,tex2d2);
		vkCmdBindDescriptorSets(frame.commandBuffer,VK_PIPELINE_BIND_POINT_GRAPHICS,texture2dRenderer->pipelineLayout,0,1,&tex2binder,0,nullptr);

		vkCmdDraw(frame.commandBuffer,6,1,0,0);

		vkCmdEndRenderPass(frame.commandBuffer);
		vkEndCommandBuffer(frame.commandBuffer);

		vulkan.SubmitCommandBuffer(frame.commandBuffer);
		vulkan.ShowFrame(frame);
	}

	vkDeviceWaitIdle(vulkan.logicalDevice);
	vulkan.DestroyTexture2d(tex2d1);
	vulkan.DestroyTexture2d(tex2d2);
	vulkan.CleanUp();

	return 0;
}
