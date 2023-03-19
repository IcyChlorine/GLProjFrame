#include "GameTransform.h"



GameTransform::GameTransform(GLFWwindow* window, InputManager& input)
	:cmd{cmd}//差点忘了要用这个坑爹的语法
{
	//this->cmd = cmd;
	this->window = window;
	time = (float)glfwGetTime();
	time_prev = time;

	int width, height;
	glfwGetWindowSize(window, &width, &height);
	glfwSetCursorPos(window, width / 2, height / 2);
}


GameTransform::~GameTransform()
{
}


glm::mat4 GameTransform::getTransformMat()
{
	return glm::mat4();
}


void GameTransform::apply(Shader& shader)
{	
	if (enabled)
		update();

	unsigned int transformLoc = glGetUniformLocation(shader.m_shaderID, "transform");
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transMat));
}


void GameTransform::update()
{
	time = (float)glfwGetTime();
	//只有时间间隔足够大时才更新变换，避免因鼠标只移动了一两个像素造成的不平滑
	if (time - time_prev < 0.01f)
		return;

	//旋转部分
	int width, height;
	double cursor_x, cursor_y;
	glfwGetWindowSize(window, &width, &height);
	glfwGetCursorPos(window, &cursor_x, &cursor_y);
	//phi += (time - time_prev)*10;
	phi += (cursor_x - width / 2)*phi_rate * 360;
	th += (cursor_y - height / 2)*th_rate * 90;
	//th = 0.0f;
	if (th >= 90.0f) th = 90.0f;
	if (th <= -90.0f) th = -90.0f;
	
	//平移部分
	glm::vec4 dr = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		dr.x += (time - time_prev)*vx;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		dr.x -= (time - time_prev)*vx;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		dr.z += (time - time_prev)*vz;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		dr.z -= (time - time_prev)*vz;
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		dr.y += (time - time_prev)*vy;
	if (glfwGetKey(window, GLFW_KEY_CAPS_LOCK) == GLFW_PRESS)
		dr.y -= (time - time_prev)*vy;
	
	//要从本地坐标系变换到世界坐标系，变换是反着的，所以右乘
	dr = dr * glm::rotate(glm::mat4(1.0f), glm::radians(phi), glm::vec3(0.0f, 1.0f, 0.0f));
	x += dr.x;
	y += dr.y;
	z += dr.z;

	//计算变换
	transMat = glm::mat4(1.0f);
	transMat = glm::rotate(transMat, glm::radians(th), glm::vec3(1.0f, 0.0f, 0.0f));
	transMat = glm::rotate(transMat, glm::radians(phi), glm::vec3(0.0f, 1.0f, 0.0f));
	transMat = glm::translate(transMat, glm::vec3(-x, -y, -z));
	glfwSetCursorPos(window, width / 2, height / 2);

	time_prev = time;
}


void GameTransform::enable(bool enabled)
{
	this->enabled = enabled;
}
