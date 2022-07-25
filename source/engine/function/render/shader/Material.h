#pragma once
#include <string>
#include <vector>
#include "Shader.h"

struct MaterialAttribute
{
public:
	MaterialAttribute(std::string varName, std::size_t id)
		: mVarName(varName), mTextureID(id)
	{

	}
	std::string mVarName;
	std::size_t mTextureID;
};

class Material
{
public:
	Material();
	Material(std::string name,Shader* shader, const std::vector<MaterialAttribute>& attribute);

private:
	void createDescriptorSet();

public:
	vk::DescriptorSet mDescriptorSet;

private:
	Shader* mShader;
	std::string mName;
	std::vector<MaterialAttribute> mMaterialAttribute;
};

