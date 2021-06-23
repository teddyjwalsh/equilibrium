#pragma once

#include "GLFW/glfw3.h"

#include "component.h"

class CompGraphics : public Component
{
public:
	GLFWwindow* window;

	void get_window_size(int& width, int& height)
	{
		glfwGetWindowSize(window, &width, &height);
	}
};