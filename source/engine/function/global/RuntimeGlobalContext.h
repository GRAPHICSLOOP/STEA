#pragma once
#include <memory>
#include "function/render/RenderSystem.h"
#include "function/render/WindowSystem.h"
#include "core/log/LogSystem.h"


// 管理生命周期，并且创建/销毁所有的全局系统
class RuntimeGlobalContext
{
public:
	void startSystems();
	void shutdownSystems();

	RenderSystem* getRenderSystem();
	LogSystem* getLogSystem();
	WindowSystem* getWindowSystem();
	VulkanRHI* getRHI();
	RenderResource* getRenderResource();

private:
	std::shared_ptr<WindowSystem> mWindowSystem;
	std::shared_ptr<RenderSystem> mRenderSystem;
	std::shared_ptr<LogSystem> mLogSystem;

};

extern RuntimeGlobalContext gRuntimeGlobalContext;
