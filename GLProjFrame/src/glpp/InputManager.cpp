#include "glpp/InputManager.h"
InputManager* InputManager::instance{ nullptr };

void InputManager::glfw_key_callback(GLFWwindow * window, int key_num, int scancode, int action, int mods)
{										
	if (instance->key_callback)//ÅÐ¶Ïº¯ÊýÊÇ·ñÎª¿Õ
		instance->key_callback(key_num, action);
}

void InputManager::glfw_mouse_button_callback(GLFWwindow * window, int button, int action, int mods)
{
	if(instance->mouse_click_callback)
		instance->mouse_click_callback(button, action);
}

void InputManager::glfw_mouse_move_callback(GLFWwindow * window, double x, double y)
{
	if (instance->mouse_move_callback)
		instance->mouse_move_callback(x, y);
}

void InputManager::init(GLFWwindow* window)
{
	glfwSetKeyCallback(window, glfw_key_callback);
	glfwSetMouseButtonCallback(window, glfw_mouse_button_callback);
	glfwSetCursorPosCallback(window, glfw_mouse_move_callback);
}

InputManager::InputManager(GLFWwindow* window)
{
	if (instance) {
		cerr << "InputManager.Constructor: Instance already exists!" << endl;
		return;
	}
	init(window);
	instance = this;
}


InputManager::~InputManager()
{
}

void InputManager::registerMouseClickCallback(function<void(int, int)> func) {
	mouse_click_callback = func;
}
void InputManager::registerMouseMoveCallback(function<void(float, float)> func) {
	mouse_move_callback = func;
}
void InputManager::registerKeyCallback(function<void(int, int)> func) {
	key_callback = func;
}
