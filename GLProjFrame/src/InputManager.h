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
	static constexpr int KEY_ARR_CAPACITY = 512; // 384=GLFW_KEY_LAST keys defined by glfw
	static constexpr int KEY_PRESS = 0;
	static constexpr int KEY_RELEASE = 1;
	static constexpr int MOUSE_PRESS = 0;
	static constexpr int MOUSE_RELEASE = 1;
	static constexpr int MOUSE_LEFT = 0;
	static constexpr int MOUSE_RIGHT = 1;
	static constexpr int MOUSE_MIDDLE = 2;
	// key press|release
	// mouse move
	// mouse click(left, middle, right; press, release)
	// mouse scroll

	//UserInput* input;
	vector<function<void()>> mouse_click_callbacks[3][2];//[0|1][0|1]表示[左键|右键|中键][按下|抬起]时调用的函数
	vector<function<void(float, float)>> mouse_move_callbacks;
	vector<function<void()>> key_callbacks[KEY_ARR_CAPACITY][2];//[GLFW_KEY][0|1]表示某个键[按下|抬起]时调用的函数
	vector<function<void(float)>> scroll_callbacks;


	static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	static void glfw_mouse_move_callback(GLFWwindow* window, double x, double y);
	static void glfw_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	void init(GLFWwindow * window);
public:
	InputManager(GLFWwindow* window);
	~InputManager();

	void addMouseClickCallback(function<void()> func, int key, int action);
	void addMouseMoveCallback(function<void(float, float)> func);
	void addKeyCallback(function<void()> func, int key, int action);
	void addMouseScrollCallback(function<void(float)> func);
};

