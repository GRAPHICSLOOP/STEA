#include "InputSystem.h"
#include "function/global/RuntimeGlobalContext.h"
#include "core/base/macro.h"

void InputSystem::listenKeyInput()
{
	gRuntimeGlobalContext.getWindowSystem()->registerOnKeyFunc(std::bind(&InputSystem::onKey,
		this,
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3,
		std::placeholders::_4));

}

void InputSystem::listenMouseInput()
{
	gRuntimeGlobalContext.getWindowSystem()->registerOnMouseButtonFunc(std::bind(&InputSystem::onMouseButton,
		this,
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3));

	gRuntimeGlobalContext.getWindowSystem()->registerOnCursorPosFunc(std::bind(&InputSystem::onCursorPos,
		this,
		std::placeholders::_1,
		std::placeholders::_2));
}

void InputSystem::setMouseCursorMode(bool enable)
{
	gRuntimeGlobalContext.getWindowSystem()->setMouseCursorMode(enable);
}
