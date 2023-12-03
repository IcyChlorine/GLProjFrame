#pragma once

#include "common_t1.h"

class InputManager;

class Window :
	public AbsObject
{
protected:
	GLFWwindow* internal_wnd_pointer{ nullptr };
	InputManager* inputManager{ nullptr };

	bool is_fullscreen{ false };
public:
	Window(AbsObject* father=nullptr);
	~Window();

	GLFWwindow* getInternalPointer() { return this->internal_wnd_pointer; }
	bool shouldClose() { return glfwWindowShouldClose(internal_wnd_pointer); }
	void getSize(int* width, int* height) { glfwGetWindowSize(internal_wnd_pointer, width, height); }
	void setCursorPos(int x, int y) { glfwSetCursorPos(internal_wnd_pointer, x, y); }
	void toggleFullscreen();

	InputManager* getInputManager() { return inputManager; }
};

