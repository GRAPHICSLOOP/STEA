#include "Material.h"
#include <algorithm>
#include "../renderResource/ImageResource.h"
#include "../../global/RuntimeGlobalContext.h"

Material::Material()
{

}

Material::Material(std::string name, Shader* shader, const std::vector<MaterialAttribute>& attribute)
	:mName(name), mShader(shader), mMaterialAttribute(attribute)
{
	createDescriptorSet();
}

void Material::createDescriptorSet()
{
	std::vector<std::string> varNames;
	for (const auto& at : mMaterialAttribute)
	{
		varNames.push_back(at.mVarName);
	}
	mDescriptorSet = mShader->generateSet(varNames)[0];

	for (const auto& at : mMaterialAttribute)
	{
		ImageResource* imageResource = gRuntimeGlobalContext.getRenderResource()->mGlobalTextureResources[at.mTextureID];
		mShader->updateDescriptorSet(at.mVarName, mDescriptorSet, &imageResource->mImageBufferResource.mImageInfo, nullptr);
	}
}

