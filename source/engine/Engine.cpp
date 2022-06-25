#include "Engine.h"
#include "core/base/macro.h"

void Engine::startEngine(const EngineConfigParams& params)
{
	gRuntimeGlobalContext.startSystems();

	STEALOG_INFO("start engine!!!");
}

void Engine::shutdownEngine()
{

}

void Engine::run()
{
	WindowSystem* windowSystem = gRuntimeGlobalContext.getWindowSystem();
	while (!windowSystem->shouldClose())
	{
		static auto startTime = std::chrono::high_resolution_clock::now();
		auto currentTime = std::chrono::high_resolution_clock::now();
		float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		windowSystem->pollEvents();
		logicalTick(deltaTime);
		rendererTick(deltaTime);
		clear();
	}
}

void Engine::initialize()
{

}

void Engine::clear()
{

}


void Engine::logicalTick(float deltaTime)
{

}

void Engine::rendererTick(float deltaTime)
{

}

void Engine::calculateFPS(float deltaTime)
{

}
