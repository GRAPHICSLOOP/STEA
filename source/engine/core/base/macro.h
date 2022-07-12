#pragma once
#include "function/global/RuntimeGlobalContext.h"


#define LOG_HELPER(LOG_LEVEL,...) \
	gRuntimeGlobalContext.getLogSystem()->log(LOG_LEVEL,"[" + std::string(__FUNCTION__) + "]" + __VA_ARGS__);


#define STEALOG_DEBUG(...) LOG_HELPER(LOG_LEVEL::LEVEL_DEBUG,__VA_ARGS__);
#define STEALOG_ERROR(...) LOG_HELPER(LOG_LEVEL::LEVEL_ERROR,__VA_ARGS__);
#define STEALOG_INFO(...) LOG_HELPER(LOG_LEVEL::LEVEL_INFO,__VA_ARGS__);
#define STEALOG_WARN(...) LOG_HELPER(LOG_LEVEL::LEVEL_WARN,__VA_ARGS__);

#define CHECK_NULL(i) \
	if(!i)\
	{ \
		throw std::runtime_error(#i "is null!"); \
	}


