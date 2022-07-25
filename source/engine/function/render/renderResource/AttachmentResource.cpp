#include "AttachmentResource.h"

AttachmentResource::AttachmentResource()
{
}

AttachmentResource::AttachmentResource(Shader* shader, const std::vector<AttachmentAttribute>& attachmentAttribute)
	:mAttachmentAttribute(attachmentAttribute)
{
	createDescriptorSet(shader);
}

const ImageResource* AttachmentResource::getImageResource(std::string varName)
{
	for (const auto& at : mAttachmentAttribute)
	{
		if (varName == at.mVarName)
			return at.mImageResource.get();
	}

	return nullptr;
}

void AttachmentResource::createDescriptorSet(Shader* shader)
{
	// 先生成set
	std::vector<std::string> varNames;
	for (const auto& at : mAttachmentAttribute)
	{
		varNames.push_back(at.mVarName);
	}
	mDescriptorSet = shader->generateSet(varNames)[0];

	// 更新set
	for (const auto& at : mAttachmentAttribute)
	{
		shader->updateDescriptorSet(at.mVarName, mDescriptorSet, &getImageResource(at.mVarName)->mImageBufferResource.mImageInfo, nullptr);
	}
}

