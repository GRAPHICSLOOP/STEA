#include "TransformComponent.h"
#include "glm/gtc/matrix_transform.hpp"

TransformComponent::TransformComponent(std::shared_ptr<class Object> obj)
	:Component(obj)
{
	dirty = true;
	setPosistion(glm::vec3(0.f));
	setScale(glm::vec3(1.f));
	setRotation(Rotation());
}

void TransformComponent::setPosistion(glm::vec3 pos)
{
	mPosition = pos;
	mTranslateMatrix = glm::translate(glm::mat4(1.f), mPosition);
	dirty = true;
}

void TransformComponent::setRotation(Rotation rot)
{
	mRotation = rot;
	glm::mat4 mat_yaw = glm::rotate(glm::mat4(1.f), glm::radians(mRotation.yaw), glm::vec3(0.f, 1.f, 0.f));
	glm::mat4 mat_pich = glm::rotate(glm::mat4(1.f), glm::radians(mRotation.pitch), glm::vec3(0.f, 0.f, 1.f));
	glm::mat4 mat_roll = glm::rotate(glm::mat4(1.f), glm::radians(mRotation.roll), glm::vec3(1.f, 0.f,0.f));

	mRotationMatrix = mat_roll * mat_pich * mat_yaw;
	dirty = true;
}

void TransformComponent::setScale(glm::vec3 scale)
{
	mScale = scale;
	mScaleMatrix = glm::scale(glm::mat4(1.f), mScale);
	dirty = true;
}

glm::vec3 TransformComponent::getPosistion()
{
	return mPosition;
}

Rotation TransformComponent::getRotation()
{
	return mRotation;
}

glm::vec3 TransformComponent::getScale()
{
	return mScale;
}

glm::mat4 TransformComponent::getModelMatrix()
{
	if (dirty)
	{
		mModelMatrix = mTranslateMatrix * mRotationMatrix * mScaleMatrix;
		dirty = false;
	}

	return mModelMatrix;
}
