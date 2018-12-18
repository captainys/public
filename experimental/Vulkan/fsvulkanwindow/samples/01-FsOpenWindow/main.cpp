#include <math.h>

#include "fssimplewindow.h"
#include "fsvulkan.h"


int main(void)
{
	FsOpenWindow(0,0,800,600,1);

	auto &vulkan=FsVulkanContext::GetContext();

	double t=0;
	FsPassedTime();
	for(;;)
	{
		float r=(float)(1.0+sin(t))/2.0f,g=(float)(1.0+cos(t*1.5))/2.0f,b=(float)(1.0+sin(t*0.7))/2.0f;

		FsPollDevice();
		auto key=FsInkey();
		if(FSKEY_ESC==key)
		{
			break;
		}

		auto &frame=vulkan.primarySwapChain.ReadyFrame();
		frame.BeginCommandBuffer();

		auto renderPassInfo=vulkan.primarySwapChain.MakeRenderPassBeginInfo(frame);
		renderPassInfo.clearColor[0]={r,g,b,0};
		vkCmdBeginRenderPass(frame.commandBuffer,&renderPassInfo.beginInfo,VK_SUBPASS_CONTENTS_INLINE);
		vkCmdEndRenderPass(frame.commandBuffer);
		vkEndCommandBuffer(frame.commandBuffer);

		vulkan.SubmitCommandBuffer(frame.commandBuffer);
		vulkan.ShowFrame(frame);

		t+=(double)FsPassedTime()/1000.0;
	}

	vulkan.CleanUp();

	return 0;
}
