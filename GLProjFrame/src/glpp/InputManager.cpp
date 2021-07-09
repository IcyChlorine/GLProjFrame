#include "glpp/InputManager.h"
InputManager* instance{ nullptr };

void InputManager::glfw_key_callback(GLFWwindow * window, int key_num, int scancode, int action, int mods)
{
	if (key_num < 0 || key_num >= KEY_ARR_CAPACITY) {
		cerr << "InputManager::glfw_key_callback: 无法处理的按键！" << endl;
		return;
	}
	//										判断函数是否为空
	if (action == GLFW_PRESS && instance->key_callback[key_num][0])
		instance->key_callback[key_num][0]();
	else if (action == GLFW_RELEASE && instance->key_callback[key_num][1])
		instance->key_callback[key_num][1]();
}

void InputManager::glfw_mouse_button_callback(GLFWwindow * window, int button, int action, int mods)
{
	if (action == GLFW_PRESS) {
		if (button == GLFW_MOUSE_BUTTON_LEFT && instance->mouse_click_callback[0][0])
			instance->mouse_click_callback[0][0]();
		else if (button == GLFW_MOUSE_BUTTON_RIGHT && instance->mouse_click_callback[1][0])
			instance->mouse_click_callback[1][0]();
	}
	else if (action == GLFW_RELEASE) {
		if (button == GLFW_MOUSE_BUTTON_LEFT && instance->mouse_click_callback[0][1])
			instance->mouse_click_callback[0][1]();
		else if (button == GLFW_MOUSE_BUTTON_RIGHT && instance->mouse_click_callback[1][1])
			instance->mouse_click_callback[1][1]();
	}
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

void InputManager::setMouseClickCallback(function<void()> func, int key, int action) {
	if ((key != 0 && key != 1) ||
		(action !=0 && action!=1)) {
		cerr << "InputManager.setMouseClickCallback: Illegal Argument!";
		return;
	}
	mouse_click_callback[key][action] = func;
}
void InputManager::setMouseMoveCallback(function<void(float, float)> func) {
	mouse_move_callback = func;
}
void InputManager::setKeyCallback(function<void()> func, int key, int action) {
	if ((key < 0 || key>=KEY_ARR_CAPACITY) ||
		(action != 0 && action != 1)) {
		cerr << "InputManager.setKeyCallback: Illegal Argument!";
		return;
	}
	key_callback[key][action] = func;
}
