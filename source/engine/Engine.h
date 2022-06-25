#pragma once
#include <filesystem>

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

};

