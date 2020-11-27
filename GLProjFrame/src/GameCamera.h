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

	//Ϊ�˿��Ʊ任��ʱ�� ��λ����
	//����һ��update�����ʱ��
	float cumulated_time{ 0.0f };
	//��ǰ�������λ��(in WC) ��λ��GL��׼����ϵ
	float x{ 0.0f }, y{ 0.0f }, z{ 0.0f };
	/*�ؼ�����ת�Ƕȡ�
	*���ڲ��õ�����Ϸ�еľ�������任�����ֻ��������ת�͸�������������
	*	��Ҳ��ŷ�����е�ǰ�����Ƕȣ���˲���������Ԫ��
	*th(theta): �����ǣ�����Ϊ��������Ϊ������Χ-90~+90
	*phi: ������ת�ǣ���תΪ������תΪ������Χ����������
	*��λ���Ƕ���
	*/
	float th{ 0.0f }, phi{ 0.0f };
	//��������ʱ������ƶ����ٶ� ��λ��GL��׼����ϵ/��
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

	virtual void outputDebugInfo(ostream& out);
};

