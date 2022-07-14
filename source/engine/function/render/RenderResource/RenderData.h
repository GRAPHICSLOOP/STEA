#pragma once
#include "glm/glm.hpp"
#include <vulkan/vulkan.hpp>
#include <vector>

enum DESCRIPTOR_TYPE
{
	DT_ObjectUniform = 0,
	DT_CamearUniform = 1,
	DT_Sample = 2,
	DT_PushConstantsUniform = 3,
	DT_Count = 4
};

struct VertexBufferData
{
public:
	glm::vec3 mPosition;
	glm::vec3 mNormal;
	glm::vec2 mTexCoord;
};

struct CameraBufferData
{
public:
	glm::mat4 mView;
	glm::mat4 mProj;
	glm::mat4 mViewPorj;
	glm::vec3 mLightPos;
	float mPaddingSpecularStrengthl;
	glm::vec3 mEyePos;
	float mPaddingPow;
};

struct ObjectBufferData
{
public:
	glm::mat4 mModel;
};

enum class IMAGE_LAYOUT_BARRIER
{
	Undefined,
	TransferDest,
	ColorAttachment,
	DepthStencilAttachment,
	TransferSource,
	Present,
	PixelShaderRead,
	PixelDepthStencilRead,
	ComputeGeneralRW,
	PixelGeneralRW,
};


