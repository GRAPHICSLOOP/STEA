#pragma once
#include <memory>
#include "pass/MainCameraPass.h"

class RenderPipeline
{
public:
	void initialize();
	void draw();
private:
	std::shared_ptr<MainCameraPass> mCameraPass;
};

