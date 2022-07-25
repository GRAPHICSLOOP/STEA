#pragma once
#include "ImageResource.h"

struct AttachmentAttribute
{
public:
	AttachmentAttribute(std::shared_ptr<ImageResource> imageResource, std::string varName)
		:mImageResource(imageResource), mVarName(varName)
	{

	}
	std::shared_ptr<ImageResource> mImageResource;
	std::string mVarName;
};

class AttachmentResource
{
public:
	AttachmentResource();
	AttachmentResource(Shader* shader,const std::vector<AttachmentAttribute>& attachmentAttribute);
	const ImageResource* getImageResource(std::string varName);


public:
	vk::DescriptorSet mDescriptorSet;

private:
	void createDescriptorSet(Shader* shader);

private:
	std::vector<AttachmentAttribute> mAttachmentAttribute;
};

