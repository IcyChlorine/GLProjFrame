#include "glpp/GameCamera.h"
#include "glpp/Window.h"
#include "glpp/Application.h"


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

	//旋转部分
	auto window = ((Application*)father)->getWindow()->getInternalPointer();
	int width, height;
	double cursor_x, cursor_y;
	glfwGetWindowSize(window, &width, &height);
	glfwGetCursorPos(window, &cursor_x, &cursor_y);

	phi += -(cursor_x - width / 2)*phi_rate * 360;
	th += (cursor_y - height / 2)*th_rate * 90;
	//截断
	if (th >= 180.0f) th = 180.0f;
	if (th <= 0.0f) th = 0.0f;

	//平移部分
	glm::vec4 dr = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		dr.x += dt*vx;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		dr.x -= dt*vx;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		dr.z += dt*vz;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		dr.z -= dt*vz;
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		dr.y += dt*vy;
	if (glfwGetKey(window, GLFW_KEY_CAPS_LOCK) == GLFW_PRESS)
		dr.y -= dt*vy;

	float phi_rad = glm::radians(phi);
	float th_rad = glm::radians(th);

	const auto UP = glm::vec3(0.,1.,0.);
	auto& vPos = cameraPos;
	auto vFwd = glm::vec3(sin(phi_rad)*sin(th_rad),cos(th_rad),cos(phi_rad)*sin(th_rad));
	auto vRight = glm::cross(vFwd, UP);

	vPos += +dr.z*vFwd + dr.x*vRight + dr.y*UP;
	//计算变换
	auto transMat = glm::lookAt(vPos, vPos+vFwd, UP);
	
	transform->setTransformMat(transMat);
	glfwSetCursorPos(window, width / 2, height / 2);

}

void GameCamera::update(glm::vec3 dr_local, float d_th, float d_phi){
	phi += d_phi;
	th += d_th;
	//截断
	if (th >= 180.0f) th = 180.0f;
	if (th <= 0.0f) th = 0.0f;

	// 中间变量
	float phi_rad = glm::radians(phi);
	float th_rad = glm::radians(th);
	const auto UP = glm::vec3(0.,1.,0.);
	auto& vPos = cameraPos;
	auto vFwd = glm::vec3(sin(phi_rad)*sin(th_rad),cos(th_rad),cos(phi_rad)*sin(th_rad));
	auto vRight = glm::cross(vFwd, UP);

	//计算变换 ——能用向量就不用矩阵 ——lcj
	vPos += +dr_local.z*vFwd + dr_local.x*vRight + dr_local.y*UP;	
	auto transMat = glm::lookAt(vPos, vPos+vFwd, UP);
	
	transform->setTransformMat(transMat);
}

void GameCamera::move(glm::vec3 dr_local){
	// 中间变量
	float phi_rad = glm::radians(phi);
	float th_rad = glm::radians(th);
	const auto UP = glm::vec3(0.,1.,0.);
	auto& vPos = cameraPos;
	auto vFwd = glm::vec3(sin(phi_rad)*sin(th_rad),cos(th_rad),cos(phi_rad)*sin(th_rad));
	auto vRight = glm::cross(vFwd, UP);

	//计算变换 ——能用向量就不用矩阵 ——lcj
	vPos += +dr_local.z*vFwd + dr_local.x*vRight + dr_local.y*UP;	
	auto transMat = glm::lookAt(vPos, vPos+vFwd, UP);
	
	transform->setTransformMat(transMat);
}
void GameCamera::rotate(float d_th, float d_phi){
	phi += d_phi;
	th += d_th;
	//截断
	if (th >= 180.0f) th = 180.0f;
	if (th <= 0.0f) th = 0.0f;

	// 中间变量
	float phi_rad = glm::radians(phi);
	float th_rad = glm::radians(th);
	const auto UP = glm::vec3(0.,1.,0.);
	auto& vPos = cameraPos;
	auto vFwd = glm::vec3(sin(phi_rad)*sin(th_rad),cos(th_rad),cos(phi_rad)*sin(th_rad));
	auto vRight = glm::cross(vFwd, UP);

	//计算变换 ——能用向量就不用矩阵 ——lcj
	auto transMat = glm::lookAt(vPos, vPos+vFwd, UP);
	
	transform->setTransformMat(transMat);
}

void GameCamera::outputDebugInfo(ostream & out)
{
	char str[100];
	sprintf_s(str, 100, "(x, y, z) = (%.2f, %.2f, %.2f)\n(phi, th)=(%.2f, %.2f)\n", 
		cameraPos.x, cameraPos.y, cameraPos.z,
		phi, th);
	out << str;
}
