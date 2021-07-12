#include "glpp/GameCamera.h"
#include "glpp/Window.h"
#include "glpp/Application.h"


GameCamera::GameCamera(AbsObject* father) :Camera{ father } {
}

GameCamera::~GameCamera()
{
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
	dr_local = v_move*dr_local;
	// 中间变量
	float phi_rad = glm::radians(phi);
	float th_rad = glm::radians(th);
	const auto UP = glm::vec3(0.,1.,0.);
	auto& vPos = cameraPos;
	auto vFwd = glm::vec3(sin(phi_rad)*sin(th_rad),cos(th_rad),cos(phi_rad)*sin(th_rad));
	auto vRight = glm::cross(vFwd, UP);

	//计算变换 ——能用向量就不用矩阵 ——lcj
	vPos += (+dr_local.z*vFwd + dr_local.x*vRight + dr_local.y*UP);	
	auto transMat = glm::lookAt(vPos, vPos+vFwd, UP);
	
	transform->setTransformMat(transMat);
}
void GameCamera::rotate(float d_th, float d_phi){
	phi += v_rotate*d_phi;
	th += v_rotate*d_th;
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
