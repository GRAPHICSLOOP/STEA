#pragma once
#include "RenderPassBase.h"

/*
* 先直接用库去实现，后面有需要的话就直接改库
* https://github.com/ocornut/imgui/wiki/Integrating-with-Vulkan
*
*/

struct UIPassConfigParam
{
public:
	UIPassConfigParam() {};
	UIPassConfigParam(vk::RenderPass pass)
		:mRenderPass(pass)
	{

	}

public:
	vk::RenderPass mRenderPass;
};

class UIPass 
{
public:
	~UIPass();
	void initialize(const UIPassConfigParam& config);
	void drawPass();

public:
	UIPassConfigParam mConfig;

protected:
	void initializeUIRenderBackend();

private:
	void uploadFonts();
};

