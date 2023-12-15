#include "Application.h"
#include "MyApp.h"
using namespace std;

void handleEvents(GLFWwindow *window)
{
	//if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	//	glfwSetWindowShouldClose(window, true);
}

/* theApp should be accessible from any object, so make it global.*/
GraphicsApplication* theApp{ nullptr };

int main() {
	try {
		theApp = new DemoApp();
		theApp->run();
		
		delete theApp;
		theApp = nullptr;
	}
	catch (init_exception& e) {
		HandleExceptionOnline(e, "init_exception");
	}
	catch (exception& e) {
		HandleExceptionOnline(e);
	}

	return 0;
}

