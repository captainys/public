#include <vector>
#include <string>

#include "fssimplewindow.h"
#include "fsvulkan.h"



void FsVulkanContext::Renderer3dBase::CmdSetModelView(Frame &frame,const float tfm[16])
{
	vkCmdPushConstants(frame.commandBuffer,this->pipelineLayout,VK_SHADER_STAGE_VERTEX_BIT,0,sizeof(float)*16,tfm);
}



