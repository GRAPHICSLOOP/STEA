#include "RenderCamera.h"
#include <glm/gtc/matrix_transform.hpp>

RenderCamera::RenderCamera()
{
	mRotation.yaw = -180.f;
	mRotation.pitch = 0.f;
	mRotation.roll = 0.f;

	mForwradVector = -Z;
	mPosition = glm::vec3(0, 0, 4.f);
}

void RenderCamera::move(glm::vec3 delta)
{
	mPosition += delta.x * getRightVector();
	mPosition += delta.y * getForwardVector();
	mPosition += delta.z * getUpVector();
}

void RenderCamera::rotate(glm::vec2 delta)
{
	if (mRotation.pitch + delta.y > 90.f || mRotation.pitch + delta.y < -90.f)
	{
		delta.y = 0;
	}

	mRotation.pitch += delta.y;
	mRotation.yaw -= delta.x;

	glm::vec3 dir;
	dir.x = glm::cos(glm::radians(mRotation.yaw / 2.f)) * glm::cos(glm::radians(mRotation.pitch / 2.f));
	dir.y = glm::sin(glm::radians(mRotation.pitch / 2.f));
	dir.z = glm::sin(glm::radians(mRotation.yaw / 2.f)) * glm::cos(glm::radians(mRotation.pitch / 2.f));
	mForwradVector = glm::normalize(dir);
}

glm::mat4 RenderCamera::getViewMatrix()
{
	return glm::lookAtRH(mPosition, mPosition + getForwardVector(), getUpVector());
}

glm::vec3 RenderCamera::getUpVector()
{
	return glm::normalize(glm::cross(getForwardVector(), getRightVector()));
}

glm::vec3 RenderCamera::getRightVector()
{
	return glm::normalize(glm::cross(Y, getForwardVector()));
}

glm::vec3 RenderCamera::getForwardVector()
{
	return mForwradVector;
}

glm::vec3 RenderCamera::getPosition()
{
	return mPosition;
}
