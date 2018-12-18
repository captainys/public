#include <math.h>
#include <chrono>

#include "ysglmath.h"

#include "fssimplewindow.h"
#include "fsvulkan.h"



int MakeSphere(GLfloat vtx[],GLfloat nom[],GLfloat col[],int nh,int nv)
{
	int nVtx=0;
	for(int v=0; v<nv; v++)
	{
		const double v0=-YSGLPI/2.0+YSGLPI*(double)v/(double)nv;
		const double v1=-YSGLPI/2.0+YSGLPI*(double)(v+1)/(double)nv;

		for(int h=0; h<nh; h++)
		{
			const int colIndex=1+(v*2/nv)+(h*4/nh)*2;

			const double h0=YSGLPI*2.0*(double)h/(double)nh;
			const double h1=YSGLPI*2.0*(double)(h+1)/(double)nh;

			vtx[nVtx*3  ]=(GLfloat)(cos(v0)*cos(h0));
			vtx[nVtx*3+1]=(GLfloat)(sin(v0));
			vtx[nVtx*3+2]=(GLfloat)(cos(v0)*sin(h0));
			col[nVtx*4  ]=(GLfloat)(colIndex&1);
			col[nVtx*4+1]=(GLfloat)((colIndex>>1)&1);
			col[nVtx*4+2]=(GLfloat)((colIndex>>2)&1);
			col[nVtx*4+3]=1;
			nVtx++;

			vtx[nVtx*3  ]=(GLfloat)(cos(v0)*cos(h1));
			vtx[nVtx*3+1]=(GLfloat)(sin(v0));
			vtx[nVtx*3+2]=(GLfloat)(cos(v0)*sin(h1));
			col[nVtx*4  ]=(GLfloat)(colIndex&1);
			col[nVtx*4+1]=(GLfloat)((colIndex>>1)&1);
			col[nVtx*4+2]=(GLfloat)((colIndex>>2)&1);
			col[nVtx*4+3]=1;
			nVtx++;

			vtx[nVtx*3  ]=(GLfloat)(cos(v1)*cos(h0));
			vtx[nVtx*3+1]=(GLfloat)(sin(v1));
			vtx[nVtx*3+2]=(GLfloat)(cos(v1)*sin(h0));
			col[nVtx*4  ]=(GLfloat)(colIndex&1);
			col[nVtx*4+1]=(GLfloat)((colIndex>>1)&1);
			col[nVtx*4+2]=(GLfloat)((colIndex>>2)&1);
			col[nVtx*4+3]=1;
			nVtx++;

			vtx[nVtx*3  ]=(GLfloat)(cos(v0)*cos(h1));
			vtx[nVtx*3+1]=(GLfloat)(sin(v0));
			vtx[nVtx*3+2]=(GLfloat)(cos(v0)*sin(h1));
			col[nVtx*4  ]=(GLfloat)(colIndex&1);
			col[nVtx*4+1]=(GLfloat)((colIndex>>1)&1);
			col[nVtx*4+2]=(GLfloat)((colIndex>>2)&1);
			col[nVtx*4+3]=1;
			nVtx++;

			vtx[nVtx*3  ]=(GLfloat)(cos(v1)*cos(h1));
			vtx[nVtx*3+1]=(GLfloat)(sin(v1));
			vtx[nVtx*3+2]=(GLfloat)(cos(v1)*sin(h1));
			col[nVtx*4  ]=(GLfloat)(colIndex&1);
			col[nVtx*4+1]=(GLfloat)((colIndex>>1)&1);
			col[nVtx*4+2]=(GLfloat)((colIndex>>2)&1);
			col[nVtx*4+3]=1;
			nVtx++;

			vtx[nVtx*3  ]=(GLfloat)(cos(v1)*cos(h0));
			vtx[nVtx*3+1]=(GLfloat)(sin(v1));
			vtx[nVtx*3+2]=(GLfloat)(cos(v1)*sin(h0));
			col[nVtx*4  ]=(GLfloat)(colIndex&1);
			col[nVtx*4+1]=(GLfloat)((colIndex>>1)&1);
			col[nVtx*4+2]=(GLfloat)((colIndex>>2)&1);
			col[nVtx*4+3]=1;
			nVtx++;
		}
	}

	for(int i=0; i<nVtx; i++)
	{
		nom[i*3  ]=vtx[i*3  ];
		nom[i*3+1]=vtx[i*3+1];
		nom[i*3+2]=vtx[i*3+2];
	}

	return nVtx;
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

const float cubeTexCoord[]=
{
	0,0, 0,0, 0,0 ,0,0 ,0,0, 0,0,
	0,0, 0,0, 0,0 ,0,0 ,0,0, 0,0,
	0,0, 0,0, 0,0 ,0,0 ,0,0, 0,0,
	0,0, 0,0, 0,0 ,0,0 ,0,0, 0,0,
	0,0, 0,0, 0,0 ,0,0 ,0,0, 0,0,
	0,0, 0,0, 0,0 ,0,0 ,0,0, 0,0,
};

const float floorVtx[6*3]=
{
	-10,0,-10,
	 10,0,-10,
	 10,0, 10,

	 10,0, 10,
	-10,0, 10,
	-10,0,-10,
};
const float floorNom[6*3]=
{
	0,1,0,
	0,1,0,
	0,1,0,

	0,1,0,
	0,1,0,
	0,1,0,
};
const float floorCol[6*4]=
{
	1,1,0,1,
	1,1,0,1,
	1,1,0,1,
	1,1,0,1,
	1,1,0,1,
	1,1,0,1,
};




class Program
{
public:
	float rot;

	float lightDir[4];

	float lightDistOffset;
	float lightDistScaling;
	float shadowMapTfmFull[YSVULKAN_SHADOWMAP_MAX_NUM_TEXTURE][16];

	int nSphereVtx;
	float sphereVtx[3*3*2*8*16],sphereNom[3*3*2*8*16],sphereCol[3*4*2*8*16];

	void GetSceneTransformation(float projection[16],float modelview[16],float rot);
	void GetShadowTransformation(float projection[16],float modelview[16],const float lightDir[3],int bufferIdent);

	void DrawScene(
	    FsVulkanContext &vulkan,
	    FsVulkanContext::Frame &frame,
	    FsVulkanContext::Shaded3dRenderer *renderer,
	    FsVulkanContext::Texture2d shadowTexture[YSVULKAN_SHADOWMAP_MAX_NUM_TEXTURE],
	    const float viewTfm[16]);
	void DrawVertexBuffer(
	    FsVulkanContext &vulkan,
	    FsVulkanContext::Frame &frame,
	    FsVulkanContext::Shaded3dRenderer *renderer,
	    int nVtx,FsVulkanContext::Buffer vtxBuf,FsVulkanContext::Buffer colBuf,FsVulkanContext::Buffer nomBuf,FsVulkanContext::Buffer texCoordBuf,
	    float x,float y,float z,float h,float p,float b,const float viewTfm[16]);
	void DrawFloor(FsVulkanContext &vulkan,FsVulkanContext::Frame &frame,FsVulkanContext::Shaded3dRenderer *renderer,const float viewTfm[16]);

	void RenderShadow(
		FsVulkanContext &context,
	    FsVulkanContext::RenderToTexture &shadowMap,
	    FsVulkanContext::Shaded3dRenderer *renderer,
	    int bufferId,
	    const float lightDir[3]);
};

void Program::GetSceneTransformation(float projection[16],float modelview[16],float rot)
{
	int wid,hei;
	FsGetWindowSize(wid,hei);

	const double aspect=(double)wid/(double)hei;

	YsGLMakePerspectivefv(projection,YSGLPI/6.0,aspect,1.0,50.0);

	YsGLMakeIdentityfv(modelview);
	YsGLMultMatrixTranslationfv(modelview,0.0f,0.0f,-10.0f);
	YsGLMultMatrixRotationZYfv(modelview,-YSGLPI/4.0);
	YsGLMultMatrixRotationXZfv(modelview,rot);
	YsGLMultMatrixTranslationfv(modelview,0.0f,-2.0f,0.0f);

}
void Program::GetShadowTransformation(float projection[16],float modelview[16],const float lightDir[3],int bufferIdent)
{
	float yVec[3];
	YsGLGetArbitraryPerpendicularVectorfv(yVec,lightDir);

	float xVec[3];
	YsGLCrossProductfv(xVec,yVec,lightDir);

	float rotMat[16]=
	{
		xVec[0],yVec[0],lightDir[0],  0,
		xVec[1],yVec[1],lightDir[1],  0,
		xVec[2],yVec[2],lightDir[2],  0,
		0,0,0,1
	};

	if(0==bufferIdent)
	{
		YsGLMakeOrthographicfv(projection,-1,1,-1,1,1.0,20);
	}
	else if(1==bufferIdent)
	{
		YsGLMakeOrthographicfv(projection,-2,2,-2,2,1.0,20);
	}
	else
	{
		YsGLMakeOrthographicfv(projection,-10,10,-10,10,1.0,20);
	}

	YsGLMakeTranslationfv(modelview,0,0,-15.0f);
	YsGLMultMatrixfv(modelview,modelview,rotMat);
	YsGLMultMatrixTranslationfv(modelview,0,-2,0);
}

void Program::DrawScene(
	FsVulkanContext &vulkan,
	FsVulkanContext::Frame &frame,
	FsVulkanContext::Shaded3dRenderer *renderer,
    FsVulkanContext::Texture2d shadowTexture[YSVULKAN_SHADOWMAP_MAX_NUM_TEXTURE],
	const float viewTfm[16])
{
	auto cubeVtxBuf=frame.CreatePerFrameVertexBuffer(sizeof(cubeVtx),cubeVtx);
	auto cubeColBuf=frame.CreatePerFrameVertexBuffer(sizeof(cubeCol),cubeCol);
	auto cubeNomBuf=frame.CreatePerFrameVertexBuffer(sizeof(cubeNom),cubeNom);
	auto cubeTexCoordBuf=frame.CreatePerFrameVertexBuffer(sizeof(cubeTexCoord),cubeTexCoord);

	auto sphereVtxBuf=frame.CreatePerFrameVertexBuffer(sizeof(sphereVtx),sphereVtx);
	auto sphereColBuf=frame.CreatePerFrameVertexBuffer(sizeof(sphereCol),sphereCol);
	auto sphereNomBuf=frame.CreatePerFrameVertexBuffer(sizeof(sphereNom),sphereNom);
	auto sphereTexCoordBuf=frame.CreatePerFrameVertexBuffer(sizeof(sphereVtx),sphereVtx); // Dummy

	auto texBind=frame.CreatePerFrameSamplerBinder(*renderer,1,vulkan.GetStockAllZeroTexture2d());

	auto shadowBind=frame.CreatePerFrameSamplerBinder(*renderer,2,4,shadowTexture);

	vkCmdBindPipeline(frame.commandBuffer,VK_PIPELINE_BIND_POINT_GRAPHICS,renderer->triangleListPipeline);

	// Light0 is perfectly fine.  Just testing.
	float dir4d[4];
	dir4d[0]=lightDir[0];
	dir4d[1]=lightDir[1];
	dir4d[2]=lightDir[2];
	dir4d[3]=0.0;

	YsGLMultMatrixVectorfv(dir4d,viewTfm,dir4d);

	float l=sqrt(dir4d[0]*dir4d[0]+dir4d[1]*dir4d[1]+dir4d[2]*dir4d[2]);

	const int usingLight=2;
	renderer->currentState.lightEnabled[usingLight]=1;
	renderer->currentState.lightPos    [usingLight][0]=dir4d[0]/l;
	renderer->currentState.lightPos    [usingLight][1]=dir4d[1]/l;
	renderer->currentState.lightPos    [usingLight][2]=dir4d[2]/l;
	renderer->currentState.lightPos    [usingLight][3]=0;
	renderer->currentState.lightColor  [usingLight][0]=1;
	renderer->currentState.lightColor  [usingLight][1]=1;
	renderer->currentState.lightColor  [usingLight][2]=1;
	renderer->currentState.ambientColor[0]=0.2f;
	renderer->currentState.ambientColor[1]=0.2f;
	renderer->currentState.ambientColor[2]=0.2f;
	renderer->currentState.specularColor[0]=1;
	renderer->currentState.specularColor[1]=1;
	renderer->currentState.specularColor[2]=1;
	renderer->currentState.specularExponent=100.0f;



	float lightDistOffset=0.001;
	float lightDistScaling=1.01;

	float viewInv[16],shadowMapTfm[16];
	YsGLInvertMatrixfv(viewInv,viewTfm);

	renderer->currentState.useShadowMap[0]=1;
	renderer->currentState.useShadowMap[1]=1;
	renderer->currentState.useShadowMap[2]=1;
	renderer->currentState.useShadowMap[3]=0;
	for(int i=0; i<YSVULKAN_SHADOWMAP_MAX_NUM_TEXTURE; ++i)
	{
		YsGLMultMatrixfv(renderer->currentState.shadowMapTransform[i],shadowMapTfmFull[i],viewInv);
		renderer->currentState.lightDistOffset[i]=lightDistOffset;
		renderer->currentState.lightDistScale[i]=lightDistScaling;
	}



	VkDescriptorSet uniformBinder[]=
	{
		renderer->CreateCurrentStateUniformBinder(frame)
	};
	vkCmdBindDescriptorSets(frame.commandBuffer,VK_PIPELINE_BIND_POINT_GRAPHICS,renderer->pipelineLayout,0,1,uniformBinder,0,nullptr);
	vkCmdBindDescriptorSets(frame.commandBuffer,VK_PIPELINE_BIND_POINT_GRAPHICS,renderer->pipelineLayout,1,1,&texBind,0,nullptr);
	vkCmdBindDescriptorSets(frame.commandBuffer,VK_PIPELINE_BIND_POINT_GRAPHICS,renderer->pipelineLayout,2,1,&shadowBind,0,nullptr);


	DrawVertexBuffer(vulkan,frame,renderer,36,cubeVtxBuf,cubeColBuf,cubeNomBuf,cubeTexCoordBuf,-1.5f,2,-1.5f,0,0,0,viewTfm);
	DrawVertexBuffer(vulkan,frame,renderer,36,cubeVtxBuf,cubeColBuf,cubeNomBuf,cubeTexCoordBuf, 1.5f,2, 1.5f,0,0,0,viewTfm);

	DrawVertexBuffer(vulkan,frame,renderer,nSphereVtx,sphereVtxBuf,sphereColBuf,sphereNomBuf,sphereTexCoordBuf,3.0f,5.0f,0.0f,0,0,0,viewTfm);

	DrawVertexBuffer(vulkan,frame,renderer,nSphereVtx,sphereVtxBuf,sphereColBuf,sphereNomBuf,sphereTexCoordBuf, 1.5f,2,-1.5f,0,0,0,viewTfm);
	DrawVertexBuffer(vulkan,frame,renderer,nSphereVtx,sphereVtxBuf,sphereColBuf,sphereNomBuf,sphereTexCoordBuf,-1.5f,2, 1.5f,0,0,0,viewTfm);
}

void Program::DrawVertexBuffer(
    FsVulkanContext &vulkan,
    FsVulkanContext::Frame &frame,
    FsVulkanContext::Shaded3dRenderer *renderer,
    int nVtx,FsVulkanContext::Buffer vtxBuf,FsVulkanContext::Buffer colBuf,FsVulkanContext::Buffer nomBuf,FsVulkanContext::Buffer texCoordBuf,
    float x,float y,float z,float h,float p,float b,const float viewTfm[16])
{
	float tfm[16];
	for(int i=0; i<16; ++i)
	{
		tfm[i]=viewTfm[i];
	}
	YsGLMultMatrixTranslationfv(tfm,x,y,z);
	YsGLMultMatrixRotationXZfv(tfm,h);
	YsGLMultMatrixRotationYZfv(tfm,p);
	YsGLMultMatrixRotationXYfv(tfm,b);

	VkBuffer toBind[]={vtxBuf.buf,colBuf.buf,nomBuf.buf,texCoordBuf.buf};
	VkDeviceSize toBindOffset[]={0,0,0,0};
	vkCmdBindVertexBuffers(frame.commandBuffer,0,sizeof(toBind)/sizeof(toBind[0]),toBind,toBindOffset);

	renderer->CmdSetModelView(frame,tfm);
	vkCmdDraw(frame.commandBuffer,nVtx,1,0,0);
}

void Program::DrawFloor(FsVulkanContext &vulkan,FsVulkanContext::Frame &frame,FsVulkanContext::Shaded3dRenderer *renderer,const float viewTfm[16])
{
	auto vtxBuf=frame.CreatePerFrameVertexBuffer(sizeof(floorVtx),floorVtx);
	auto colBuf=frame.CreatePerFrameVertexBuffer(sizeof(floorCol),floorCol);
	auto nomBuf=frame.CreatePerFrameVertexBuffer(sizeof(floorNom),floorNom);
	auto texCoordBuf=frame.CreatePerFrameVertexBuffer(sizeof(floorVtx),floorVtx);

	VkBuffer toBind[]={vtxBuf.buf,colBuf.buf,nomBuf.buf,texCoordBuf.buf};
	VkDeviceSize toBindOffset[]={0,0,0,0};
	vkCmdBindVertexBuffers(frame.commandBuffer,0,sizeof(toBind)/sizeof(toBind[0]),toBind,toBindOffset);

	renderer->CmdSetModelView(frame,viewTfm);
	vkCmdDraw(frame.commandBuffer,6,1,0,0);
}



void Program::RenderShadow(
		FsVulkanContext &vulkan,
	    FsVulkanContext::RenderToTexture &shadowMap,
	    FsVulkanContext::Shaded3dRenderer *renderer,
	    int bufferId,
	    const float lightDir[3])
{
	auto &frame=shadowMap.frame[bufferId];
	auto cmdBuf=frame.commandBuffer;

	frame.DestroyAllPerFrameResource();
	frame.BeginCommandBuffer();

	auto renderPassInfo=shadowMap.MakeRenderPassBeginInfo(frame);
	vkCmdBeginRenderPass(cmdBuf,&renderPassInfo.beginInfo,VK_SUBPASS_CONTENTS_INLINE);

	float projection[16],modelview[16];
	GetShadowTransformation(projection,modelview,lightDir,bufferId);

	YsGLMultMatrixfv(shadowMapTfmFull[bufferId],projection,modelview);



	for(int i=0; i<16; ++i)
	{
		renderer->currentState.projection[i]=projection[i];
	}
	FsVulkanContext::Texture2d dummyShadowTexture[YSVULKAN_SHADOWMAP_MAX_NUM_TEXTURE]=
	{
		vulkan.GetStockAllZeroTexture2d(),
		vulkan.GetStockAllZeroTexture2d(),
		vulkan.GetStockAllZeroTexture2d(),
		vulkan.GetStockAllZeroTexture2d(),
	};
	DrawScene(vulkan,frame,renderer,dummyShadowTexture,modelview);

	vkCmdEndRenderPass(cmdBuf);

	vkEndCommandBuffer(cmdBuf);
}

int main(void)
{
	Program prog;

	FsOpenWindow(0,0,800,600,1);

	auto &vulkan=FsVulkanGetContext();
	auto shaded3dRenderer=vulkan.primaryRendererPool.CreateShaded3dRenderer(vulkan.primarySwapChain.renderPass);
	auto texture2dRenderer=vulkan.primaryRendererPool.CreateTexture2dTriangleRenderer(vulkan.primarySwapChain.renderPass);

	prog.nSphereVtx=MakeSphere(prog.sphereVtx,prog.sphereNom,prog.sphereCol,16,8);
	prog.rot=0.0;

	FsVulkanContext::RendererPool shadowMapRendererPool(&vulkan);

	std::vector <FsVulkanContext::RenderToTexture> shadowMap;
	shadowMap.resize(vulkan.primarySwapChain.frame.size());
	for(auto &s : shadowMap)
	{
		s.CreateShadowMap(&vulkan,3,2048,2048);
	}
	auto shadow3dRenderer=shadowMapRendererPool.CreateShaded3dRenderer(shadowMap[0].renderPass);



	int showDepthTexture=0;
	float lightDir[3]={1,1,0};
	{
		float l=sqrt(lightDir[0]*lightDir[0]+lightDir[1]*lightDir[1]+lightDir[2]*lightDir[2]);
		lightDir[0]/=l;
		lightDir[1]/=l;
		lightDir[2]/=l;
	}
	prog.lightDir[0]=lightDir[0];
	prog.lightDir[1]=lightDir[1];
	prog.lightDir[2]=lightDir[2];
	prog.lightDir[3]=0;



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




		// Rendering Shadow Map >>
		auto &usingShadowMap=shadowMap[frame.imageIndex];
		{
			prog.RenderShadow(vulkan,usingShadowMap,shadow3dRenderer,0,lightDir);
			prog.RenderShadow(vulkan,usingShadowMap,shadow3dRenderer,1,lightDir);
			prog.RenderShadow(vulkan,usingShadowMap,shadow3dRenderer,2,lightDir);

			vulkan.SubmitCommandBuffer(
				{
					usingShadowMap.frame[0].commandBuffer,
					usingShadowMap.frame[1].commandBuffer,
					usingShadowMap.frame[2].commandBuffer,
				},
				vulkan.imageAvailableSemaphore,
				usingShadowMap.renderFinishedSemaphore);
		}
		// Rendering Shadow Map <<



		frame.BeginCommandBuffer();


		auto renderPassInfo=vulkan.primarySwapChain.MakeRenderPassBeginInfo(frame);
		renderPassInfo.clearColor[0]={0.7f,0.7f,1.0f,0.0f};
		vkCmdBeginRenderPass(frame.commandBuffer,&renderPassInfo.beginInfo,VK_SUBPASS_CONTENTS_INLINE);



		float modelView[16];
		prog.GetSceneTransformation(shaded3dRenderer->currentState.projection,modelView,heading);



		FsVulkanContext::Texture2d shadowTexture[YSVULKAN_SHADOWMAP_MAX_NUM_TEXTURE]=
		{
			usingShadowMap.GetDepthTexture2d(0),
			usingShadowMap.GetDepthTexture2d(1),
			usingShadowMap.GetDepthTexture2d(2),
			usingShadowMap.GetDepthTexture2d(2),
		};
		prog.DrawScene(vulkan,frame,shaded3dRenderer,shadowTexture,modelView);
		prog.DrawFloor(vulkan,frame,shaded3dRenderer,modelView);


		{
			float texTriVtx2d[]=
			{
				  0,  0,
				200,  0,
				  0,200,

				  0,200,
				200,200,
				200,  0,
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
			auto tex1binder=frame.CreatePerFrameSamplerBinder(*texture2dRenderer,0,usingShadowMap.GetDepthTexture2d(showDepthTexture));
			vkCmdBindDescriptorSets(frame.commandBuffer,VK_PIPELINE_BIND_POINT_GRAPHICS,texture2dRenderer->pipelineLayout,0,1,&tex1binder,0,nullptr);

			texture2dRenderer->CmdUsePixelCoordTopLeftAsOrigin(frame);

			vkCmdDraw(frame.commandBuffer,6,1,0,0);
		}



		vkCmdEndRenderPass(frame.commandBuffer);

		vkEndCommandBuffer(frame.commandBuffer);

		vulkan.SubmitCommandBuffer(frame.commandBuffer,usingShadowMap.renderFinishedSemaphore);
		vulkan.ShowFrame(frame);

		double PI=3.14159265359;
		double t=(double)FsPassedTime()/1000.0;


		if(FsGetKeyState(FSKEY_SPACE))
		{
			heading+=t*PI*1.5;
		}
		if(FSKEY_ENTER==key)
		{
			showDepthTexture=(showDepthTexture+1)%3;
		}
	}

	vkDeviceWaitIdle(vulkan.logicalDevice);
	for(auto &s : shadowMap)
	{
		s.DestroyRenderToTexture();
	}
	shadowMapRendererPool.DestroyRenderer();
	vulkan.CleanUp();

	return 0;
}
