#include <chrono>

#include "ysglmath.h"

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



const float cubeVtx[]=
{
	-1,-1,-1,  // 6 quads split into all triangles = 12 triangles
	 1,-1,-1,  // 12 triangles times 3 vertices = 36 vertices
	 1, 1,-1,  // 36 vertices times 3 components = 108 floats

	 1, 1,-1,
	-1, 1,-1,
	-1,-1,-1,

	-1,-1, 1,
	 1,-1, 1,
	 1, 1, 1,

	 1, 1, 1,
	-1, 1, 1,
	-1,-1, 1,

	-1,-1,-1,
	 1,-1,-1,
	 1,-1, 1,

	 1,-1, 1,
	-1,-1, 1,
	-1,-1,-1,
       
	-1, 1,-1,
	 1, 1,-1,
	 1, 1, 1,

	 1, 1, 1,
	-1, 1, 1,
	-1, 1,-1,

	-1,-1,-1,
	-1, 1,-1,
	-1, 1, 1,

	-1, 1, 1,
	-1,-1, 1,
	-1,-1,-1,
       
	 1,-1,-1,
	 1, 1,-1,
	 1, 1, 1,

	 1, 1, 1,
	 1,-1, 1,
	 1,-1,-1,
};

const float cubeNom[]=
{
	0,0,-1, 0,0,-1, 0,0,-1, 0,0,-1, 0,0,-1, 0,0,-1,  // 36 vertices x 3 floats = 108 floats
	0,0, 1, 0,0, 1, 0,0, 1, 0,0, 1, 0,0, 1, 0,0, 1, 
	0,-1,0, 0,-1,0, 0,-1,0, 0,-1,0, 0,-1,0, 0,-1,0, 
	0, 1,0, 0, 1,0, 0, 1,0, 0, 1,0, 0, 1,0, 0, 1,0, 
	-1,0,0, -1,0,0, -1,0,0, -1,0,0, -1,0,0, -1,0,0, 
	 1,0,0,  1,0,0,  1,0,0,  1,0,0,  1,0,0,  1,0,0, 
};

const float cubeCol[]=
{
	1,0,0,1, 1,0,0,1, 1,0,0,1, 1,0,0,1, 1,0,0,1, 1,0,0,1, // Colors 36 vertices times 4 components = 144 floats
	1,0,0,1, 1,0,0,1, 1,0,0,1, 1,0,0,1, 1,0,0,1, 1,0,0,1, 
	0,1,0,1, 0,1,0,1, 0,1,0,1, 0,1,0,1, 0,1,0,1, 0,1,0,1, 
	0,1,0,1, 0,1,0,1, 0,1,0,1, 0,1,0,1, 0,1,0,1, 0,1,0,1, 
	0,0,1,1, 0,0,1,1, 0,0,1,1, 0,0,1,1, 0,0,1,1, 0,0,1,1, 
	0,0,1,1, 0,0,1,1, 0,0,1,1, 0,0,1,1, 0,0,1,1, 0,0,1,1, 
};



const float lineVtx[]=
{
	-1.1f,-1.1f,-1.1f, -1.1f,-1.1f, 1.1f, 
	 1.1f,-1.1f,-1.1f,  1.1f,-1.1f, 1.1f, 
	 1.1f, 1.1f,-1.1f,  1.1f, 1.1f, 1.1f, 
	-1.1f, 1.1f,-1.1f, -1.1f, 1.1f, 1.1f, 

	-1.1f,-1.1f,-1.1f, -1.1f, 1.1f,-1.1f, 
	 1.1f,-1.1f,-1.1f,  1.1f, 1.1f,-1.1f, 
	 1.1f,-1.1f, 1.1f,  1.1f, 1.1f, 1.1f, 
	-1.1f,-1.1f, 1.1f, -1.1f, 1.1f, 1.1f, 

	-1.1f,-1.1f,-1.1f,  1.1f,-1.1f,-1.1f, 
	-1.1f, 1.1f,-1.1f,  1.1f, 1.1f,-1.1f, 
	-1.1f, 1.1f, 1.1f,  1.1f, 1.1f, 1.1f, 
	-1.1f,-1.1f, 1.1f,  1.1f,-1.1f, 1.1f, 
};

const float lineCol[]=
{
	1,1,1,1, 1,1,1,1, 
	1,1,1,1, 1,1,1,1, 
	1,1,1,1, 1,1,1,1, 
	1,1,1,1, 1,1,1,1, 

	1,1,1,1, 1,1,1,1, 
	1,1,1,1, 1,1,1,1, 
	1,1,1,1, 1,1,1,1, 
	1,1,1,1, 1,1,1,1, 

	1,1,1,1, 1,1,1,1, 
	1,1,1,1, 1,1,1,1, 
	1,1,1,1, 1,1,1,1, 
	1,1,1,1, 1,1,1,1, 
};


int main(void)
{
	FsOpenWindow(0,0,800,600,1);

	auto &vulkan=FsVulkanGetContext();
	auto plain3dRenderer=vulkan.primaryRendererPool.CreatePlain3dRenderer(vulkan.primarySwapChain.renderPass);

	auto rgba1=EightColorToRGBA(256,::bmp1);
	auto tex2d1=vulkan.CreateTexture2d(16,16,rgba1.data());

	auto rgba2=EightColorToRGBA(256,::bmp2);
	auto tex2d2=vulkan.CreateTexture2d(16,16,rgba2.data());

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





		auto triVtxBuf=frame.CreatePerFrameVertexBuffer(sizeof(cubeVtx),cubeVtx);
		auto triColBuf=frame.CreatePerFrameVertexBuffer(sizeof(cubeCol),cubeCol);

		auto lineVtxBuf=frame.CreatePerFrameVertexBuffer(sizeof(lineVtx),lineVtx);
		auto lineColBuf=frame.CreatePerFrameVertexBuffer(sizeof(lineCol),lineCol);


		const double PI=3.14159265359;
		float projection[16],modelView[16];
		YsGLMakePerspectivefv(projection,PI/4.0,double(wid)/double(hei),0.1,100.0);

		YsGLMakeIdentityfv(modelView);
		YsGLMultMatrixTranslationfv(modelView,0.0,0.0,-3.0);
		YsGLMultMatrixRotationZYfv(modelView,pitch);
		YsGLMultMatrixRotationXZfv(modelView,heading);

		vkCmdBindPipeline(frame.commandBuffer,VK_PIPELINE_BIND_POINT_GRAPHICS,plain3dRenderer->triangleListPipeline);

		VkBuffer toBind[]={triVtxBuf.buf,triColBuf.buf};
		VkDeviceSize toBindOffset[]={0,0};

		vkCmdBindVertexBuffers(frame.commandBuffer,0,sizeof(toBind)/sizeof(toBind[0]),toBind,toBindOffset);

		VkDescriptorSet uniformBinder[]=
		{
			frame.CreatePerFrameUniformBinder(*plain3dRenderer,0,sizeof(projection),projection),
		};
		plain3dRenderer->CmdSetModelView(frame,modelView);
		vkCmdBindDescriptorSets(frame.commandBuffer,VK_PIPELINE_BIND_POINT_GRAPHICS,plain3dRenderer->pipelineLayout,0,1,uniformBinder,0,nullptr);
		vkCmdDraw(frame.commandBuffer,36,1,0,0);


		vkCmdBindPipeline(frame.commandBuffer,VK_PIPELINE_BIND_POINT_GRAPHICS,plain3dRenderer->lineListPipeline);

		VkBuffer lineToBind[]={lineVtxBuf.buf,lineColBuf.buf};
		VkDeviceSize lineToBindOffset[]={0,0};
		vkCmdBindVertexBuffers(frame.commandBuffer,0,sizeof(lineToBind)/sizeof(lineToBind[0]),lineToBind,lineToBindOffset);
		vkCmdDraw(frame.commandBuffer,24,1,0,0);




		vkCmdEndRenderPass(frame.commandBuffer);
		vkEndCommandBuffer(frame.commandBuffer);

		vulkan.SubmitCommandBuffer(frame.commandBuffer);
		vulkan.ShowFrame(frame);

		double t=(double)FsPassedTime()/1000.0;
		pitch+=t*PI;
		heading+=t*PI*1.5;
	}

	vkDeviceWaitIdle(vulkan.logicalDevice);
	vulkan.DestroyTexture2d(tex2d1);
	vulkan.DestroyTexture2d(tex2d2);
	vulkan.CleanUp();

	return 0;
}
