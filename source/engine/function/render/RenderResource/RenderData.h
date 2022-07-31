#pragma once
#include "glm/glm.hpp"
#include <vulkan/vulkan.hpp>
#include <vector>

#define LIGHT_MAXNUMB 100

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

struct LightBufferData
{
public:
	glm::vec4 mPosition;
	glm::vec3 mColor;
	float mRadius;
};

struct LightInfo
{
public:
	glm::vec3 mPosition;
	glm::vec3 mDirection;
	float mSpeed;
};

struct BoundingBox
{
public:
	glm::vec3 mMin;
	glm::vec3 mMax;

	BoundingBox()
		: mMin(0.f, 0.f, 0.f)
		, mMax(1.f, 1.f, 1.f)
	{

	}

	BoundingBox(const glm::vec3& inMin, const glm::vec3& inMax)
		: mMin(inMin)
		, mMax(inMax)
	{

	}
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


