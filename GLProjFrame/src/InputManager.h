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
	GLFWwindow* window { nullptr };
	static constexpr int KEY_ARR_CAPACITY = 512; // 384=GLFW_KEY_LAST keys defined by glfw
	static constexpr int KEY_PRESS = 0;
	static constexpr int KEY_RELEASE = 1;
	static constexpr int MOUSE_PRESS = 0;
	static constexpr int MOUSE_RELEASE = 1;
	static constexpr int MOUSE_LEFT = 0;
	static constexpr int MOUSE_RIGHT = 1;
	static constexpr int MOUSE_MIDDLE = 2;
	// Inputs to support:
	// key press|release
	// mouse move
	// mouse click(left|middle|right x press|release)
	// mouse scroll
	int debug_counter{ 0 };

	// maintain status of keys and mouse buttons
	int wnd_width{ -1 }, wnd_height{ -1 };
	bool keys[KEY_ARR_CAPACITY]{ false };
	bool mouse_buttons[3]{ false };
	float mouse_x{ 0.0f }, mouse_y{ 0.0f };

	inline const bool* getKeys() const { return keys; }
	inline bool isKeyPressed(int key) const { return keys[key]; }

	// mouse is the physical device, cursor is the pointer in your window
	inline bool isMousePressed(int button) const { return mouse_buttons[button]; }
	inline void getCursorPos(float& x, float& y) const { x = mouse_x; y = mouse_y; }
	inline glm::vec2 getCursorPos() const { return glm::vec2(mouse_x, mouse_y); } 

	inline void getWindowSize(int& width, int& height) const { width = wnd_width; height = wnd_height; } 
	inline glm::vec2 getWindowSize() const { return glm::vec2(wnd_width, wnd_height); }
	
	// set cursor position, for first person camera
	// Note: glfwPollEvents 从系统负责派发的事件队列中取事件，可能会慢半拍，导致mouse_x|y更新不及时
	//       so we update mouse_x and mouse_y here for internal cursor pos updates
	inline void setCursorPos(int x, int y) { glfwSetCursorPos(window, x, y); mouse_x=x; mouse_y=y; }	

	// call backs
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

