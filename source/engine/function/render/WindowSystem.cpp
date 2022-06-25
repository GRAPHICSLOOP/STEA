#include "WindowSystem.h"

WindowSystem::~WindowSystem()
{
	glfwDestroyWindow(mWindow);
	glfwTerminate();
}

void WindowSystem::initialize()
{
	WindowConfigParams params;

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // 禁止调整窗口大小
	mWindow = glfwCreateWindow(params.mWidth, params.mHeight, params.mTitle, nullptr, nullptr);


	// 设置回调
	glfwSetWindowUserPointer(mWindow, this);
	glfwSetKeyCallback(mWindow, keyCallBack);
	glfwSetMouseButtonCallback(mWindow, mouseButtonCallback);
	glfwSetCursorPosCallback(mWindow, cursorPosCallback);
}

void WindowSystem::pollEvents() const
{
	glfwPollEvents();
}

bool WindowSystem::shouldClose() const
{
	return glfwWindowShouldClose(mWindow);
}

void WindowSystem::setMouseCursorMode(bool enable)
{
	if (enable)
	{
		glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	else
	{
		glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
}

GLFWwindow* WindowSystem::getWindow() const
{
	return mWindow;
}