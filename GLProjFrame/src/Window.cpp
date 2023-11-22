#include "Window.h"
#include "InputManager.h"

//callback func when window is resized
void glfw_resize_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

Window::Window(AbsObject* father):
	AbsObject{father}
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//create window object through glfw
	this->internal_wnd_pointer = glfwCreateWindow(800, 600, "OpenGL Framework v1.5", NULL, NULL);
	if (internal_wnd_pointer == NULL)
	{
		glfwTerminate();
		error("Failed to create GLFW window!\n");
		throw init_exception("Error: Failed to create GLFW window");
	}

	glfwMakeContextCurrent(internal_wnd_pointer);
	// Set swap inteval to 0 effectively removes the 60 fps cap,
	// which is, caused by the limited rate of window repaint
	// event issued by windows os.
	// If you want *test performance*, set the inteval to 0.
	// However, if you want to *keep your GPU quiet*, comment the 
	// following line out.
	glfwSwapInterval(0); 

	//register RESIZE listener func
	glfwSetFramebufferSizeCallback(internal_wnd_pointer, glfw_resize_callback);

	this->inputManager = new InputManager(internal_wnd_pointer);
}


Window::~Window()
{
	delete this->inputManager;
	glfwTerminate();
}
