#include "InputManager.h"
InputManager* instance{ nullptr };

void InputManager::glfw_resize_callback(GLFWwindow* window, int width, int height)
{
	for(auto& func : instance->frame_resize_callbacks)
		func(width, height);

	instance->wnd_width = width;
	instance->wnd_height = height;
}

void InputManager::glfw_key_callback(GLFWwindow * window, int key_num, int scancode, int action, int mods)
{
	if (key_num < 0 || key_num >= KEY_ARR_CAPACITY) {
		warningf("key_callback: 无法处理的按键, key_num = %d\n", key_num);
		return;
	}

	if (action == GLFW_PRESS){
		instance->keys[key_num] = true;
		for(auto& func : instance->key_callbacks[key_num][KEY_PRESS])
			func();
	} else if (action == GLFW_RELEASE) {
		instance->keys[key_num] = false;
		for(auto& func : instance->key_callbacks[key_num][KEY_RELEASE])
			func();
	}		
}

void InputManager::glfw_mouse_button_callback(GLFWwindow * window, int button, int action, int mods)
{
	if (action == GLFW_PRESS) {
		if (button == GLFW_MOUSE_BUTTON_LEFT) {
			instance->mouse_buttons[MOUSE_LEFT] = true;
			for(auto& func : instance->mouse_click_callbacks[MOUSE_LEFT][MOUSE_PRESS])
				func();
		}else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
			instance->mouse_buttons[MOUSE_RIGHT] = true;
			for(auto& func : instance->mouse_click_callbacks[MOUSE_RIGHT][MOUSE_PRESS])
				func();
		} else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
			instance->mouse_buttons[MOUSE_MIDDLE] = true;
			for(auto& func : instance->mouse_click_callbacks[MOUSE_MIDDLE][MOUSE_PRESS])
				func();
		}
	}
	else if (action == GLFW_RELEASE) {
		if (button == GLFW_MOUSE_BUTTON_LEFT) {
			instance->mouse_buttons[MOUSE_LEFT] = false;
			for(auto& func : instance->mouse_click_callbacks[MOUSE_LEFT][MOUSE_RELEASE])
				func();
		} else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
			instance->mouse_buttons[MOUSE_RIGHT] = false;
			for(auto& func : instance->mouse_click_callbacks[MOUSE_RIGHT][MOUSE_RELEASE])
				func();
		} else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
			instance->mouse_buttons[MOUSE_MIDDLE] = false;
			for(auto& func : instance->mouse_click_callbacks[MOUSE_MIDDLE][MOUSE_RELEASE])
				func();
		}
	}
}

void InputManager::glfw_mouse_move_callback(GLFWwindow * window, double x, double y)
{
	float fx = (float)x, fy = (float)y;
	instance->mouse_x = fx;
	instance->mouse_y = fy;
	for(auto& func : instance->mouse_move_callbacks)
		func(fx, fy);
}

void InputManager::glfw_scroll_callback(GLFWwindow * window, double xoffset, double yoffset)
{
	// a little note here: why there's an xoffset parameter here?
	// answer: there can be input devices like touchpad that can scroll in both x and y directions.
	for(auto& func : instance->scroll_callbacks)
		func(yoffset);
}

void InputManager::init(GLFWwindow* window)
{
	this->window = window;
	glfwSetKeyCallback        (window, glfw_key_callback);
	glfwSetMouseButtonCallback(window, glfw_mouse_button_callback);
	glfwSetCursorPosCallback  (window, glfw_mouse_move_callback);
	glfwSetScrollCallback     (window, glfw_scroll_callback);
	glfwSetFramebufferSizeCallback(window, glfw_resize_callback);

	double dx, dy;
	glfwGetCursorPos          (window, &dx, &dy);
	mouse_x = (float)dx; mouse_y = (float)dy;
	glfwGetWindowSize         (window, &wnd_width, &wnd_height);
}

InputManager::InputManager(GLFWwindow* window)
{
	if (instance) {
		error("InputManager.Constructor: Instance already exists!\n");
		throw exception();
	}
	init(window);
	instance = this;
}


InputManager::~InputManager()
{
}

void InputManager::addFrameResizeCallback(function<void(int, int)> func) {
	frame_resize_callbacks.push_back(func);
}

void InputManager::addMouseClickCallback(function<void()> func, int key, int action) {
	if ((key != 0 && key != 1) ||
		(action !=0 && action!=1)) {
		error("InputManager.addMouseClickCallback: Illegal Argument!\n");
		throw exception();
	}
	mouse_click_callbacks[key][action].push_back(func);
}

void InputManager::addMouseMoveCallback(function<void(float, float)> func) {
	mouse_move_callbacks.push_back(func);
}

void InputManager::addKeyCallback(function<void()> func, int key, int action) {
	bool valid = (key >= 0 && key < KEY_ARR_CAPACITY);
	valid = valid && (action == KEY_PRESS || action == KEY_RELEASE);
	if (!valid) {
		error("InputManager.addKeyCallback: Illegal Argument!\n");
		throw exception();
	}

	key_callbacks[key][action].push_back(func);
}

void InputManager::addMouseScrollCallback(function<void(float)> func) {
	scroll_callbacks.push_back(func);
}