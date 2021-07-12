#include <iostream>
//#include "util.h"

//»ù´¡gl¿â
#include <glad/glad.h>
#include <GLFW/glfw3.h>

//glppÀà¿â
#include "glpp/glpp.h"

using namespace std;
using namespace glm;

void handleEvents(GLFWwindow *window)
{
	//if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	//	glfwSetWindowShouldClose(window, true);
}

int main() {
	try {
		//DemoApplication* theApp = new DemoApplication;
		MeshDemo *theApp = new MeshDemo;
		theApp->run();
		delete theApp;
	}
	catch (init_exception& e) {
		HandleException(e, "init_exception");
	}
	catch (exception& e) {
		HandleException(e);
	}

	return 0;
}

