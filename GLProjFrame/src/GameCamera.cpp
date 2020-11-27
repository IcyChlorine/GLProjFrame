#include "GameCamera.h"
#include "Window.h"
#include "Application.h"


GameCamera::GameCamera(AbsObject* father) :Camera{ father } {
	int width, height;
	auto window = ((Application*)father)->getWindow();
	window->getSize(&width, &height);
	window->setCursorPos(width / 2, height / 2);
}

GameCamera::~GameCamera()
{
}

void GameCamera::update(float dt)
{
	if (!enabled) return;
	cumulated_time += dt;
	//ֻ��ʱ�����㹻��ʱ�Ÿ��±任�����������ֻ�ƶ���һ����������ɵĲ�ƽ��
	if (cumulated_time < 0.01f)
		return;

	//��ת����
	auto window = ((Application*)father)->getWindow()->getInternalPointer();
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

	//ƽ�Ʋ���
	glm::vec4 dr = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		dr.x += dt*vx;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		dr.x -= dt*vx;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		dr.z += dt*vz;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		dr.z -= dt*vz;
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		dr.y += dt*vy;
	if (glfwGetKey(window, GLFW_KEY_CAPS_LOCK) == GLFW_PRESS)
		dr.y -= dt*vy;

	//Ҫ�ӱ�������ϵ�任����������ϵ���任�Ƿ��ŵģ������ҳ�
	dr = dr * glm::rotate(glm::mat4(1.0f), glm::radians(phi), glm::vec3(0.0f, 1.0f, 0.0f));
	x += dr.x;
	y += dr.y;
	z += dr.z;

	//����任
	auto transMat = glm::mat4(1.0f);
	transMat = glm::rotate(transMat, glm::radians(th), glm::vec3(1.0f, 0.0f, 0.0f));
	transMat = glm::rotate(transMat, glm::radians(phi), glm::vec3(0.0f, 1.0f, 0.0f));
	transMat = glm::translate(transMat, glm::vec3(-x, -y, -z));
	transform->setTransformMat(transMat);
	glfwSetCursorPos(window, width / 2, height / 2);

}

void GameCamera::outputDebugInfo(ostream & out)
{
	char str[100];
	sprintf_s(str, 100, "(x, y, z) = (%.2f, %.2f, %.2f)\n(phi, th)=(%.2f, %.2f)\n", x, y, z, phi, th);
	out << str;
}
