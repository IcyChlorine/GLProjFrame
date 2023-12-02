#pragma once
#include "common_t1.h"
#include "Camera.h"
#include "Loopable.h"
class GameCamera :
	public Camera,public Loopable
{
private:
	const float pi = 3.1415926f;

	bool enabled = true;

	//为了控制变换的时间 单位：秒
	//与上一次update距离的时间
	float cumulated_time{ 0.0f };
	//当前摄像机的位置(in WC) 单位：GL标准坐标系
	glm::vec3 pos = glm::vec3(0.0f);
	/*
	 * 相机视线的方位角，采用球坐标约定
	 * 采用游戏中的经典坐标变换，因此只有左右旋转和俯仰角两个参数
	 *	这也是欧拉角中的前两个角度，因此不用引入四元数
	 * th(eta): 俯仰角，+y对应0°，-y对应180°，范围0°~180°
	 * phi: 左右旋转角，+z为0°，以y轴俯视逆时针为正方向，范围不设上下限
	 * 单位：角度制
	*/
	float th{ 90.0f }, phi{ 180.0f };
	// 相机移动的速度 单位：WC unit/s
	float vx{ 1.0f }, vy{ 1.0f }, vz{ 1.0f };
	
	/* 鼠标灵敏度
	*th_rate:默认1/pixel，表示鼠标上下移动pixel像素时俯仰角旋转90度
	*phi_rate:默认1/pixel，表示鼠标上下移动pixel像素时横向旋转360度
	*/
	float th_rate{ 1 / 1000.0f }, phi_rate{ 1 / 8000.0f };
public:
	bool accelerate{ false };

	GameCamera(AbsObject* father);
	virtual ~GameCamera();

	void setEnabled(bool enabled) { this->enabled = enabled; }
	bool getEnabled() { return enabled; }

	glm::vec3 getPosition() { return pos; }

	void getDirectionVectors(glm::vec3& front, glm::vec3& up, glm::vec3& right);

	virtual void update(float dt);

	virtual void outputDebugInfo(ostream& out);
	virtual int outputDebugInfo(char* buf);
};