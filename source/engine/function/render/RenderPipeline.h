#pragma once
#include <memory>
#include "pass/MainCameraPass.h"
#include "pass/UIPass.h"
#include "pass/PostProcessPass.h"

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
	std::shared_ptr<PostProcessPass> mPostProcessPass;
	std::shared_ptr<UIPass> mUIPass;
	Frame mFrame;
};

