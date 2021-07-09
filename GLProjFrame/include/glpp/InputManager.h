#pragma once

#include <functional>
using namespace std;

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Loopable.h"
#include "Singleton.h"
//#include "UserInput.h"
#include "Window.h"
class InputManager://not fully implemented.
	public AbsObject,public Singleton
{
public:
	static constexpr int KEY_ARR_CAPACITY = 512;

	//UserInput* input;
	function<void()> mouse_click_callback[2][2];//[0|1][0|1]表示[左键|右键][按下|抬起]时调用的函数
	function<void(float, float)> mouse_move_callback;
	function<void()> key_callback[512][2];//[GLFW_KEY][0|1]表示某个键[按下|抬起]时调用的函数

	static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	static void glfw_mouse_move_callback(GLFWwindow* window, double x, double y);
	void init(GLFWwindow * window);
public:
	InputManager(GLFWwindow* window);
	~InputManager();

	void setMouseClickCallback(function<void()> func, int key, int action);
	void setMouseMoveCallback(function<void(float, float)> func);
	void setKeyCallback(function<void()> func, int key, int action);
};

