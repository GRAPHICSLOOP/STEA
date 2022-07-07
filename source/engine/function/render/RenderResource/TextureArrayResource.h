#pragma once
#include "RenderData.h"


class TextureArrayResource
{
public:
	TextureArrayResource();
	~TextureArrayResource();

	void initialize(
		uint32_t width,
		uint32_t height,
		void* pixels,
		PIXEL_FORMAT pixelFormat,
		uint32_t miplevels,
		uint32_t layerLevels);

};

