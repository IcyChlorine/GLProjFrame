#pragma once
#include "common_t1.h"
#include "Camera.h"
#include "Loopable.h"
class GameCamera :
	public Camera,public Loopable
{
protected:
	bool enabled = true;

	//当前摄像机的位置(in WC)
	glm::vec3 cameraPos{0.,0.,0.};
	/*关键：旋转角度。
	*由于采用的是游戏中的经典坐标变换，因此只有左右旋转和俯仰角两个参数
	*	这也是欧拉角中的前两个角度，因此不用引入四元数
	*2021-7-10更新：折腾的头大，干脆改用标准极坐标算了,但坐标轴约定轮转对换一下 [x,y,z] -> [z,x,y]
	*th(theta): 俯仰角，范围[0,180] <=> 【抬头看天，低头看脚】
	*phi: 左右旋转角，方便起见，不设上下限；为了默认与GL相符（即看向-z方向），phi初始化为180°
	*单位：角度制
	*/
	float th{ 90.0f }, phi{ 180.0f };
	//按键控制时，相机移动的速度 单位：WC/s
	float vx{ 1.0f }, vy{ 1.0f }, vz{ 1.0f };
	/*鼠标控制时，相机旋转角度和鼠标移动距离的比的量度
	*th_rate:默认1/pixel，表示鼠标上下移动pixel像素时俯仰角旋转90度
	*phi_rate:默认1/pixel，表示鼠标上下移动pixel像素时横向旋转360度
	*/
	float th_rate{ 1 / 1000.0f }, phi_rate{ 1 / 8000.0f };
public:
	GameCamera(AbsObject* father);
	virtual ~GameCamera();

	void setEnabled(bool enabled) { this->enabled = enabled; }
	bool getEnabled() { return enabled; }

	virtual void update(float dt);
	virtual void update(glm::vec3 dr_local, float d_th, float d_phi);
	virtual void move(glm::vec3 dr_local);
	virtual void rotate(float d_th, float d_phi);

	virtual void outputDebugInfo(ostream& out);
};

