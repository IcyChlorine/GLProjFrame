#include "Window.h"
#include "InputManager.h"

//callback func when window is resized
void glfw_resize_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void Window::toggleFullscreen() {
	if (is_fullscreen) {
		glfwSetWindowMonitor(internal_wnd_pointer, NULL, 100, 100, 800, 600, 0);
	}
	else { // change to fullscreen
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		glfwSetWindowMonitor(internal_wnd_pointer, monitor, 0, 0, mode->width, mode->height, 0);
	}
	is_fullscreen = !is_fullscreen;
}

Window::Window(AbsObject* father):
	AbsObject{father}
{
	//TODO: glfw needs to be initialized only once...
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//TODO: make this an argument
	int nr_samples = 4;
	assert(nr_samples >= 1 and nr_samples <= 64); // prevent from wild values
	if(nr_samples > 1) {
		glfwWindowHint(GLFW_SAMPLES, 4); // MSAA
		infof("Window scheduled to do MSAA(#sp=%d), Don't forget to `glEnable(GL_MULTISAMPLE)`, or it won't work.\n", nr_samples);
	}
	
	//TODO: make window width, height, title, nr_sample and vsync configurable
	//create window via glfw
	int width{ 800 }, height{ 600 };
	const char* wnd_title = "OpenGL Framework beta";
	this->internal_wnd_pointer = glfwCreateWindow(width, height, wnd_title, NULL, NULL);
	if (internal_wnd_pointer == NULL)
	{
		glfwTerminate();
		error("Failed to create GLFW window!\n");
		throw init_exception("Error: Failed to create GLFW window");
	}

	glfwMakeContextCurrent(internal_wnd_pointer);
	/* 
	 * Swap inteval is the minimal number of screen updates that glfw
	 * will wait before swapping the buffers. Say your monitor is 120Hz,
	 * then glfw will wait <swap inteval> monitor refresh before swapping.
	 * 
	 * Set the interval to 1 is beneficial at most time, as it will eliminate
	 * redundant frames that your monitor can't keep up. However, if you
	 * want to test the performance of your program, you may want to set 
	 * swap interval to 0, allowing fps to be >= 60(or 120).
	 * 
	 * Setting swap interval to 1 is also known as `vsync`, or fps cap.
	 */
	glfwSwapInterval(1);

	//register RESIZE listener func
	glfwSetFramebufferSizeCallback(internal_wnd_pointer, glfw_resize_callback);

	this->inputManager = new InputManager(internal_wnd_pointer);
}


Window::~Window()
{
	delete this->inputManager;
	glfwTerminate();
}
