#pragma once
#include "RenderPassBase.h"
#include <unordered_map>
#include "function/render/RenderResource/RenderData.h"



class MainCameraPass : public RenderPassBase
{
public:
	void drawPass(vk::CommandBuffer cmdBuffer) override;
};
