#pragma once

#include "common_t1.h"

#include "InputManager.h"
#include "Shader.h"

class GameTransform
{
private:
	GLFWwindow* window{ nullptr };
	InputManager& cmd;

	const float pi = 3.1415926f;
	
	//为了控制变换的时间 单位：秒
	float time{ 0.0f }, time_prev{ 0.0f };
	//当前摄像机的位置(in WC) 单位：GL标准坐标系
	float x{ 0.0f }, y{ 0.0f }, z{ 0.0f };
	/*关键：旋转角度。
	*由于采用的是游戏中的经典坐标变换，因此只有左右旋转和俯仰角两个参数
	*	这也是欧拉角中的前两个角度，因此不用引入四元数
	*th(theta): 俯仰角，俯视为正，仰视为负，范围-90~+90
	*phi: 左右旋转角，右转为正，左转为负，范围不设上下限
	*单位：角度制
	*/
	float th{ 0.0f }, phi{ 0.0f };
	//按键控制时，相机移动的速度 单位：GL标准坐标系/秒
	float vx{ 1.0f }, vy{ 1.0f }, vz{ 1.0f };
	/*鼠标控制时，相机旋转角度和鼠标移动距离的比的量度
	*th_rate:默认1/pixel，表示鼠标上下移动pixel像素时俯仰角旋转90度
	*phi_rate:默认1/pixel，表示鼠标上下移动pixel像素时横向旋转360度
	*/
	float th_rate{ 1 / 1000.0f }, phi_rate{ 1 / 8000.0f };
	//变换矩阵
	glm::mat4 transMat{ glm::mat4(1.0f) };
public:
	GameTransform(GLFWwindow* window, InputManager& input);
	~GameTransform();

	bool enabled{ false };

	glm::mat4 getTransformMat();
	void apply(Shader& shader);
	void update();
	void enable(bool enabled);
};

