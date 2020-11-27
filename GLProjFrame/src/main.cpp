#include <iostream>
//#include "util.h"

//����gl��
#include <glad/glad.h>
#include <GLFW/glfw3.h>

//��ѧ���ӣ�����Ŀ�Դ���include��
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

int main() {
	try {
		Application theApp;
		theApp.run();
	}
	catch (init_exception& e) {
		HandleException(e, "init_exception");
	}
	catch (exception& e) {
		HandleException(e);
	}

	return 0;
}

