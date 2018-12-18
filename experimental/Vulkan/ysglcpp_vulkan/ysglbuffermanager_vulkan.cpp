#include "fsvulkan.h"
#include "ysglbuffermanager_vulkan.h"

////////////////////////////////////////////////////////////
YsGLBufferManager::ActualBuffer::ActualBuffer()
{
	nVtx=0;
}
YsGLBufferManager::ActualBuffer::~ActualBuffer()
{
	CleanUp();
}
void YsGLBufferManager::ActualBuffer::CleanUp(void)
{
	nVtx=0;
	auto &vulkan=FsVulkanContext::GetContext();
	vulkan.DestroyBuffer(vtxBuf);
	vulkan.DestroyBuffer(nomBuf);
	vulkan.DestroyBuffer(colBuf);
	vulkan.DestroyBuffer(pointSizeBuf);
}

void YsGLBufferManager::ActualBuffer::DrawPrimitiveVtx(VkCommandBuffer cmdBuf,const FsVulkanContext::Renderer *renderer) const
{
	int nBind=0;
	VkBuffer toBind[max_num_bind_buffer]={VK_NULL_HANDLE};
	VkDeviceSize toBindOffset[1]={0};
	for(auto b : renderer->GetAttribBinding())
	{
		switch(b)
		{
		case FsVulkanContext::Renderer::VERTEX3F:
			toBind[++nBind]=vtxBuf.buf;
			break;
		default:
			fprintf(stderr,"%s\n",__FUNCTION__);
			fprintf(stderr,"Renderer requires additional buffers.\n");
			return;
		}
		if(max_num_bind_buffer<=nBind)
		{
			fprintf(stderr,"%s\n",__FUNCTION__);
			fprintf(stderr,"Too main bindings.\n");
			return;
		}
	}
	vkCmdBindVertexBuffers(cmdBuf,0,nBind,toBind,toBindOffset);
	vkCmdDraw(cmdBuf,nVtx,1,0,0);
}
void YsGLBufferManager::ActualBuffer::DrawPrimitiveVtxNom(VkCommandBuffer cmdBuf,const FsVulkanContext::Renderer *renderer) const
{
	int nBind=0;
	VkBuffer toBind[max_num_bind_buffer]={VK_NULL_HANDLE};
	VkDeviceSize toBindOffset[1]={0};
	for(auto b : renderer->GetAttribBinding())
	{
		switch(b)
		{
		case FsVulkanContext::Renderer::VERTEX3F:
			toBind[++nBind]=vtxBuf.buf;
			break;
		case FsVulkanContext::Renderer::NORMAL3F:
			toBind[++nBind]=nomBuf.buf;
			break;
		default:
			fprintf(stderr,"%s\n",__FUNCTION__);
			fprintf(stderr,"Renderer requires additional buffers.\n");
			return;
		}
		if(max_num_bind_buffer<=nBind)
		{
			fprintf(stderr,"%s\n",__FUNCTION__);
			fprintf(stderr,"Too main bindings.\n");
			return;
		}
	}
	vkCmdBindVertexBuffers(cmdBuf,0,nBind,toBind,toBindOffset);
	vkCmdDraw(cmdBuf,nVtx,1,0,0);
}
void YsGLBufferManager::ActualBuffer::DrawPrimitiveVtxCol(VkCommandBuffer cmdBuf,const FsVulkanContext::Renderer *renderer) const
{
	int nBind=0;
	VkBuffer toBind[max_num_bind_buffer]={VK_NULL_HANDLE};
	VkDeviceSize toBindOffset[1]={0};
	for(auto b : renderer->GetAttribBinding())
	{
		switch(b)
		{
		case FsVulkanContext::Renderer::VERTEX3F:
			toBind[++nBind]=vtxBuf.buf;
			break;
		case FsVulkanContext::Renderer::COLOR4F:
			toBind[++nBind]=colBuf.buf;
			break;
		default:
			fprintf(stderr,"%s\n",__FUNCTION__);
			fprintf(stderr,"Renderer requires additional buffers.\n");
			return;
		}
		if(max_num_bind_buffer<=nBind)
		{
			fprintf(stderr,"%s\n",__FUNCTION__);
			fprintf(stderr,"Too main bindings.\n");
			return;
		}
	}
	vkCmdBindVertexBuffers(cmdBuf,0,nBind,toBind,toBindOffset);
	vkCmdDraw(cmdBuf,nVtx,1,0,0);
}
void YsGLBufferManager::ActualBuffer::DrawPrimitiveVtxNomCol(VkCommandBuffer cmdBuf,const FsVulkanContext::Renderer *renderer) const
{
	int nBind=0;
	VkBuffer toBind[max_num_bind_buffer]={VK_NULL_HANDLE};
	VkDeviceSize toBindOffset[1]={0};
	for(auto b : renderer->GetAttribBinding())
	{
		switch(b)
		{
		case FsVulkanContext::Renderer::VERTEX3F:
			toBind[++nBind]=vtxBuf.buf;
			break;
		case FsVulkanContext::Renderer::NORMAL3F:
			toBind[++nBind]=nomBuf.buf;
			break;
		case FsVulkanContext::Renderer::COLOR4F:
			toBind[++nBind]=colBuf.buf;
			break;
		default:
			fprintf(stderr,"%s\n",__FUNCTION__);
			fprintf(stderr,"Renderer requires additional buffers.\n");
			return;
		}
		if(max_num_bind_buffer<=nBind)
		{
			fprintf(stderr,"%s\n",__FUNCTION__);
			fprintf(stderr,"Too main bindings.\n");
			return;
		}
	}
	vkCmdBindVertexBuffers(cmdBuf,0,nBind,toBind,toBindOffset);
	vkCmdDraw(cmdBuf,nVtx,1,0,0);
}
void YsGLBufferManager::ActualBuffer::DrawPrimitiveVtxColPointSize(VkCommandBuffer cmdBuf,const FsVulkanContext::Renderer *renderer) const
{
	int nBind=0;
	VkBuffer toBind[max_num_bind_buffer]={VK_NULL_HANDLE};
	VkDeviceSize toBindOffset[1]={0};
	for(auto b : renderer->GetAttribBinding())
	{
		switch(b)
		{
		case FsVulkanContext::Renderer::VERTEX3F:
			toBind[++nBind]=vtxBuf.buf;
			break;
		case FsVulkanContext::Renderer::COLOR4F:
			toBind[++nBind]=colBuf.buf;
			break;
		case FsVulkanContext::Renderer::POINTSIZE1F:
			toBind[++nBind]=pointSizeBuf.buf;
			break;
		default:
			fprintf(stderr,"%s\n",__FUNCTION__);
			fprintf(stderr,"Renderer requires additional buffers.\n");
			return;
		}
		if(max_num_bind_buffer<=nBind)
		{
			fprintf(stderr,"%s\n",__FUNCTION__);
			fprintf(stderr,"Too main bindings.\n");
			return;
		}
	}
	vkCmdBindVertexBuffers(cmdBuf,0,nBind,toBind,toBindOffset);
	vkCmdDraw(cmdBuf,nVtx,1,0,0);
}

////////////////////////////////////////////////////////////

/* static */ YsGLBufferManager::ActualBuffer *YsGLBufferManager::Unit::Create(void)
{
	return new YsGLBufferManager::ActualBuffer;
}
/* static */ void YsGLBufferManager::Unit::Delete(YsGLBufferManager::ActualBuffer *ptr)
{
	if(nullptr!=ptr)
	{
		ptr->CleanUp();
		delete ptr;
	}
}

YsGLBufferManager::Unit::STATE YsGLBufferManager::Unit::GetState(void) const
{
	auto bufferPtr=GetActualBuffer();
	if(VK_NULL_HANDLE!=bufferPtr->vtxBuf.buf && VK_NULL_HANDLE!=bufferPtr->vtxBuf.mem)
	{
		return POPULATED;
	}
	return EMPTY;
}

////////////////////////////////////////////////////////////

void YsGLBufferManager::Unit::MakeVtx(YsGL::PRIMITIVE_TYPE primType,const YsGLVertexBuffer &vtxBuf,YSSIZE_T maxNumVtxPerVbo)
{
	auto nVtx=vtxBuf.GetN();
	auto &vulkan=FsVulkanContext::GetContext();
	auto actualBuffer=bufferPtr;
	actualBuffer->CleanUp();
	actualBuffer->vtxBuf=vulkan.CreateVertexBuffer(vtxBuf.GetNByte());
	vulkan.TransferToDeviceMemory(actualBuffer->vtxBuf.mem,vtxBuf.GetNByte(),vtxBuf.data());
}
void YsGLBufferManager::Unit::MakeVtxNom(YsGL::PRIMITIVE_TYPE primType,const YsGLVertexBuffer &vtxBuf,const YsGLNormalBuffer &nomBuf,YSSIZE_T maxNumVtxPerVbo)
{
	auto nVtx=vtxBuf.GetN();
	auto &vulkan=FsVulkanContext::GetContext();
	auto actualBuffer=bufferPtr;
	actualBuffer->CleanUp();
	actualBuffer->vtxBuf=vulkan.CreateVertexBuffer(vtxBuf.GetNByte());
	vulkan.TransferToDeviceMemory(actualBuffer->vtxBuf.mem,vtxBuf.GetNByte(),vtxBuf.data());
	actualBuffer->nomBuf=vulkan.CreateVertexBuffer(nomBuf.GetNByte());
	vulkan.TransferToDeviceMemory(actualBuffer->nomBuf.mem,nomBuf.GetNByte(),nomBuf.data());
}
void YsGLBufferManager::Unit::MakeVtxCol(YsGL::PRIMITIVE_TYPE primType,const YsGLVertexBuffer &vtxBuf,const YsGLColorBuffer &colBuf,YSSIZE_T maxNumVtxPerVbo)
{
	auto nVtx=vtxBuf.GetN();
	auto &vulkan=FsVulkanContext::GetContext();
	auto actualBuffer=bufferPtr;
	actualBuffer->CleanUp();
	actualBuffer->vtxBuf=vulkan.CreateVertexBuffer(vtxBuf.GetNByte());
	vulkan.TransferToDeviceMemory(actualBuffer->vtxBuf.mem,vtxBuf.GetNByte(),vtxBuf.data());
	actualBuffer->colBuf=vulkan.CreateVertexBuffer(colBuf.GetNByte());
	vulkan.TransferToDeviceMemory(actualBuffer->colBuf.mem,colBuf.GetNByte(),colBuf.data());
}
void YsGLBufferManager::Unit::MakeVtxNomCol(YsGL::PRIMITIVE_TYPE primType,const YsGLVertexBuffer &vtxBuf,const YsGLNormalBuffer &nomBuf,const YsGLColorBuffer &colBuf,YSSIZE_T maxNumVtxPerVbo)
{
	auto nVtx=vtxBuf.GetN();
	auto &vulkan=FsVulkanContext::GetContext();
	auto actualBuffer=bufferPtr;
	actualBuffer->CleanUp();
	actualBuffer->vtxBuf=vulkan.CreateVertexBuffer(vtxBuf.GetNByte());
	vulkan.TransferToDeviceMemory(actualBuffer->vtxBuf.mem,vtxBuf.GetNByte(),vtxBuf.data());
	actualBuffer->nomBuf=vulkan.CreateVertexBuffer(nomBuf.GetNByte());
	vulkan.TransferToDeviceMemory(actualBuffer->nomBuf.mem,nomBuf.GetNByte(),nomBuf.data());
	actualBuffer->colBuf=vulkan.CreateVertexBuffer(colBuf.GetNByte());
	vulkan.TransferToDeviceMemory(actualBuffer->colBuf.mem,colBuf.GetNByte(),colBuf.data());
}
void YsGLBufferManager::Unit::MakeVtxColPointSize(YsGL::PRIMITIVE_TYPE primType,const YsGLVertexBuffer &vtxBuf,const YsGLColorBuffer &colBuf,const YsGLPointSizeBuffer &psBuf,YSSIZE_T maxNumVtxPerVbo)
{
	auto nVtx=vtxBuf.GetN();
	auto &vulkan=FsVulkanContext::GetContext();
	auto actualBuffer=bufferPtr;
	actualBuffer->CleanUp();
	actualBuffer->vtxBuf=vulkan.CreateVertexBuffer(vtxBuf.GetNByte());
	vulkan.TransferToDeviceMemory(actualBuffer->vtxBuf.mem,vtxBuf.GetNByte(),vtxBuf.data());
	actualBuffer->colBuf=vulkan.CreateVertexBuffer(colBuf.GetNByte());
	vulkan.TransferToDeviceMemory(actualBuffer->colBuf.mem,colBuf.GetNByte(),colBuf.data());
	actualBuffer->pointSizeBuf=vulkan.CreateVertexBuffer(psBuf.GetNByte());
	vulkan.TransferToDeviceMemory(actualBuffer->pointSizeBuf.mem,psBuf.GetNByte(),psBuf.data());
}

YsArray <unsigned int> YsGLBufferManager::Unit::GetNativeHandle(void) const
{
	YsArray <unsigned int> hd;
	return hd;
}
