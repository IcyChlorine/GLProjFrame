#pragma once

#include "common_t1.h"

//会在循环中被循环调用的类的接口
class Loopable {
	//dt表示两次调用间的时间，单位为s
	virtual void update(float dt) = 0;
};