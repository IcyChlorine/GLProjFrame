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
	static InputManager* instance;
	//UserInput* input;
	function<void(int, int)> mouse_click_callback;//int, int->key(=LEFT|RIGHT|MID), action(=DOWN|UP)
	function<void(float, float)> mouse_move_callback;
	function<void(int, int)> key_callback;//int, int -> GLFW_KEY, action(=DOWN|UP)

	static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	static void glfw_mouse_move_callback(GLFWwindow* window, double x, double y);
	void init(GLFWwindow * window);
public:

	InputManager(GLFWwindow* window);
	~InputManager();

	void registerMouseClickCallback(function<void(int, int)> func);
	void registerMouseMoveCallback(function<void(float, float)> func);
	void registerKeyCallback(function<void(int, int)> func);
};

