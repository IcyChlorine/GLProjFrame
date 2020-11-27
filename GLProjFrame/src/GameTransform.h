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
	
	//Ϊ�˿��Ʊ任��ʱ�� ��λ����
	float time{ 0.0f }, time_prev{ 0.0f };
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
	//�任����
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

