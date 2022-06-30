#pragma once
#include <memory>
#include "pass/MainCameraPass.h"
#include "pass/UIPass.h"

class RenderPipeline
{
public:
	void initialize();
	void draw();

private:
	void beginDraw();
	void endDraw();

private:
	std::shared_ptr<MainCameraPass> mCameraPass;
	std::shared_ptr<UIPass> mUIPass;
};

