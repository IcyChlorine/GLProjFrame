#include <iostream>
//#include "util.h"

//基础gl库
#include <glad/glad.h>
#include <GLFW/glfw3.h>

//数学轮子，在项目自带的include中
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Texture.h"
#include "InputManager.h"
#include "GameTransform.h"
#include "Application.h"

using namespace std;
using namespace glm;

void handleEvents(GLFWwindow *window)
{
	//if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	//	glfwSetWindowShouldClose(window, true);
}

/* theApp should be accessible from any object, so make it global.*/
Application* theApp{nullptr};

int main() {
	try {
		theApp = new Application();
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

