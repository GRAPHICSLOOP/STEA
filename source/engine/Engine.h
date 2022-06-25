#pragma once
#include <filesystem>
#include "framework/level/Level.h"
#include "../editor/SceneInputManager.h"

struct EngineConfigParams
{
	std::filesystem::path mConfigFilePath;
};


class Engine
{

public:
	void startEngine(const EngineConfigParams& params);
	void shutdownEngine();
	void run();

	void initialize();
	void clear();
protected:
	void logicalTick(float deltaTime);
	void rendererTick(float deltaTime);

	void calculateFPS(float deltaTime);
protected:
	EngineConfigParams mInitParams;

private:
	std::shared_ptr<Level> mLevel;
	std::shared_ptr<SceneInputManager> mScenceManager;
};

