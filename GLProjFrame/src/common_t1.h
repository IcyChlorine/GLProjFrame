/*
	TIER 1 的公共引用头文件
	C,STL,
	gl,glfw,glad,glm等库的公共引用头文件
*/

#pragma once
#ifndef _COMMON_TIER1_H
#define _COMMON_TIER1_H

//C标准库
#include <iostream>
#include <string>
#include <vector>
using namespace std;

//自己写的util库
#include "util.h"
#include "logging.h"

//gl及gl工具库
#include <glad/glad.h>
#include <GLFW/glfw3.h>
//gl数学库
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#endif