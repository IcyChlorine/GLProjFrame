#pragma once

#include "common_t1.h"

//����ѭ���б�ѭ�����õ���Ľӿ�
class Loopable {
	//dt��ʾ���ε��ü��ʱ�䣬��λΪs
	virtual void update(float dt) = 0;
};