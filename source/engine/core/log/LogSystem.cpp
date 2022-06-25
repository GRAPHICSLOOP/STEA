#include "LogSystem.h"
#include <spdlog/sinks/stdout_color_sinks.h>

LogSystem::LogSystem()
{
}

LogSystem::~LogSystem()
{
	mLogger->flush();
}

void LogSystem::initialize()
{
	mLogger = spdlog::stdout_color_mt("TINY");
	mLogger->set_level(spdlog::level::trace);

	mLogger->set_pattern("[%^%l%$] %v");
}
