#include "RuntimeGlobalContext.h"
#include "function/render/VulkanRHI.h"

RuntimeGlobalContext gRuntimeGlobalContext;

void RuntimeGlobalContext::startSystems()
{
	mLogSystem = std::make_shared<class LogSystem>();
	mLogSystem->initialize();

	mWindowSystem = std::make_shared<class WindowSystem>();
	mWindowSystem->initialize();

	mRenderSystem = std::make_shared<class RenderSystem>();
	mRenderSystem->initialize();
}

void RuntimeGlobalContext::shutdownSystems()
{
}

RenderSystem* RuntimeGlobalContext::getRenderSystem() 
{
	return gRuntimeGlobalContext.mRenderSystem.get(); 
}

LogSystem* RuntimeGlobalContext::getLogSystem() 
{ 
	return gRuntimeGlobalContext.mLogSystem.get();
}

WindowSystem* RuntimeGlobalContext::getWindowSystem() 
{ 
	return gRuntimeGlobalContext.mWindowSystem.get(); 
}

VulkanRHI* RuntimeGlobalContext::getRHI()
{
	return mRenderSystem->mVulkanRHI.get();
}

RenderResource* RuntimeGlobalContext::getRenderResource()
{
	return mRenderSystem->mRenderResource.get();
}
