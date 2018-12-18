#include <chrono>

#include "ysglmath.h"

#include "fssimplewindow.h"
#include "fsvulkan.h"





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

const float cubeTexCoord[]=
{
	0,0, 0,0, 0,0 ,0,0 ,0,0, 0,0,
	0,0, 0,0, 0,0 ,0,0 ,0,0, 0,0,
	0,0, 0,0, 0,0 ,0,0 ,0,0, 0,0,
	0,0, 0,0, 0,0 ,0,0 ,0,0, 0,0,
	0,0, 0,0, 0,0 ,0,0 ,0,0, 0,0,
	0,0, 0,0, 0,0 ,0,0 ,0,0, 0,0,
};


int main(void)
{
	FsOpenWindow(0,0,800,600,1);

	auto &vulkan=FsVulkanGetContext();
	auto shaded3dRenderer=vulkan.primaryRendererPool.CreateShaded3dRenderer(vulkan.primarySwapChain.renderPass);

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
		auto triNomBuf=frame.CreatePerFrameVertexBuffer(sizeof(cubeNom),cubeNom);
		auto triTexCoordBuf=frame.CreatePerFrameVertexBuffer(sizeof(cubeTexCoord),cubeTexCoord);

		auto texBind=frame.CreatePerFrameSamplerBinder(*shaded3dRenderer,1,vulkan.GetStockAllOneTexture2d());

		FsVulkanContext::Texture2d dummyShadowMap[4]=
		{
			vulkan.GetStockAllOneTexture2d(),
			vulkan.GetStockAllOneTexture2d(),
			vulkan.GetStockAllOneTexture2d(),
			vulkan.GetStockAllOneTexture2d(),
		};
		auto shadowBind=frame.CreatePerFrameSamplerBinder(*shaded3dRenderer,2,4,dummyShadowMap);



		const double PI=3.14159265359;
		float modelView[16];
		YsGLMakePerspectivefv(shaded3dRenderer->currentState.projection,PI/4.0,double(wid)/double(hei),0.1,100.0);

		// Light0 is perfectly fine.  Just testing.
		const int usingLight=2;
		shaded3dRenderer->currentState.lightEnabled[usingLight]=1;
		shaded3dRenderer->currentState.lightPos    [usingLight][0]=0;
		shaded3dRenderer->currentState.lightPos    [usingLight][1]=0;
		shaded3dRenderer->currentState.lightPos    [usingLight][2]=1;
		shaded3dRenderer->currentState.lightPos    [usingLight][3]=0;
		shaded3dRenderer->currentState.lightColor  [usingLight][0]=1;
		shaded3dRenderer->currentState.lightColor  [usingLight][1]=1;
		shaded3dRenderer->currentState.lightColor  [usingLight][2]=1;
		shaded3dRenderer->currentState.ambientColor[0]=0.2f;
		shaded3dRenderer->currentState.ambientColor[1]=0.2f;
		shaded3dRenderer->currentState.ambientColor[2]=0.2f;
		shaded3dRenderer->currentState.specularColor[0]=1;
		shaded3dRenderer->currentState.specularColor[1]=1;
		shaded3dRenderer->currentState.specularColor[2]=1;
		shaded3dRenderer->currentState.specularExponent=100.0f;



		YsGLMakeIdentityfv(modelView);
		YsGLMultMatrixTranslationfv(modelView,0.0,0.0,-3.0);
		YsGLMultMatrixRotationZYfv(modelView,pitch);
		YsGLMultMatrixRotationXZfv(modelView,heading);

		vkCmdBindPipeline(frame.commandBuffer,VK_PIPELINE_BIND_POINT_GRAPHICS,shaded3dRenderer->triangleListPipeline);

		VkBuffer toBind[]={triVtxBuf.buf,triColBuf.buf,triNomBuf.buf,triTexCoordBuf.buf};
		VkDeviceSize toBindOffset[]={0,0,0,0};

		vkCmdBindVertexBuffers(frame.commandBuffer,0,sizeof(toBind)/sizeof(toBind[0]),toBind,toBindOffset);

		VkDescriptorSet uniformBinder[]=
		{
			shaded3dRenderer->CreateCurrentStateUniformBinder(frame)
		};
		shaded3dRenderer->CmdSetModelView(frame,modelView);
		vkCmdBindDescriptorSets(frame.commandBuffer,VK_PIPELINE_BIND_POINT_GRAPHICS,shaded3dRenderer->pipelineLayout,0,1,uniformBinder,0,nullptr);
		vkCmdBindDescriptorSets(frame.commandBuffer,VK_PIPELINE_BIND_POINT_GRAPHICS,shaded3dRenderer->pipelineLayout,1,1,&texBind,0,nullptr);
		vkCmdBindDescriptorSets(frame.commandBuffer,VK_PIPELINE_BIND_POINT_GRAPHICS,shaded3dRenderer->pipelineLayout,2,1,&shadowBind,0,nullptr);
		vkCmdDraw(frame.commandBuffer,36,1,0,0);



		vkCmdEndRenderPass(frame.commandBuffer);
		vkEndCommandBuffer(frame.commandBuffer);

		vulkan.SubmitCommandBuffer(frame.commandBuffer);
		vulkan.ShowFrame(frame);

		double t=(double)FsPassedTime()/1000.0;
		pitch+=t*PI;
		heading+=t*PI*1.5;
	}

	vkDeviceWaitIdle(vulkan.logicalDevice);
	vulkan.CleanUp();

	return 0;
}
