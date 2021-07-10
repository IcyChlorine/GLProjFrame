#pragma once
#include "common_t1.h"
#include "Camera.h"
#include "Loopable.h"
class GameCamera :
	public Camera,public Loopable
{
protected:
	bool enabled = true;

	//��ǰ�������λ��(in WC)
	glm::vec3 cameraPos{0.,0.,0.};
	/*�ؼ�����ת�Ƕȡ�
	*���ڲ��õ�����Ϸ�еľ�������任�����ֻ��������ת�͸�������������
	*	��Ҳ��ŷ�����е�ǰ�����Ƕȣ���˲���������Ԫ��
	*2021-7-10���£����ڵ�ͷ�󣬸ɴ���ñ�׼����������,��������Լ����ת�Ի�һ�� [x,y,z] -> [z,x,y]
	*th(theta): �����ǣ���Χ[0,180] <=> ��̧ͷ���죬��ͷ���š�
	*phi: ������ת�ǣ�������������������ޣ�Ϊ��Ĭ����GL�����������-z���򣩣�phi��ʼ��Ϊ180��
	*��λ���Ƕ���
	*/
	float th{ 90.0f }, phi{ 180.0f };
	//��������ʱ������ƶ����ٶ� ��λ��WC/s
	float vx{ 1.0f }, vy{ 1.0f }, vz{ 1.0f };
	/*������ʱ�������ת�ǶȺ�����ƶ�����ıȵ�����
	*th_rate:Ĭ��1/pixel����ʾ��������ƶ�pixel����ʱ��������ת90��
	*phi_rate:Ĭ��1/pixel����ʾ��������ƶ�pixel����ʱ������ת360��
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

