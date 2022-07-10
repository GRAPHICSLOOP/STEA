#pragma once
#include <memory>
#include "pass/MainCameraPass.h"
#include "pass/UIPass.h"

class RenderPipeline
{
public:
	~RenderPipeline();
	void initialize();
	void draw();

private:
	void beginDraw();
	void endDraw();

	void createAttachment();
	void createRenderPass();
	void createFrameBuffer();

private:
	std::shared_ptr<MainCameraPass> mCameraPass;
	std::shared_ptr<UIPass> mUIPass;
	Frame mFrame;
};

