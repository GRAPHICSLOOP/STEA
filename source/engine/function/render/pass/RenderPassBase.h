#pragma once
#include <vulkan/vulkan.hpp>


class RenderPassBase
{
public:
	virtual void initialize();
	virtual void drawPass();
};

