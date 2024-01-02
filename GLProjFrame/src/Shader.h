#pragma once

#ifndef SHADER_H
#define SHADER_H

/*
	Shader class from https://learnopengl.com/Getting-started/Shaders
	Modified by StarSky@PKU
*/
#include "common_t1.h"

#include <fstream>
#include <sstream>
using namespace std;

class Shader
{
public:
	// the shader program ID
	unsigned int m_shaderID;

	// constructor reads and builds the shader
	Shader(const char* vertSrcPath, const char* fragSrcPath);
	// use/activate the shader
	void use();
	// utility uniform functions
	void setUniform(const string &name, bool value);
	void setUniform(const string &name, int  value);
	void setUniform(const string &name, float value);
	void setUniform(const string &name, const glm::vec2 &value);
	void setUniform(const string &name, const glm::vec3 &value);
	void setUniform(const string &name, const glm::vec4 &value);
	void setUniformMatrix(const string &name, const glm::mat3& mat);
	void setUniformMatrix(const string &name, const glm::mat4& mat);
};

#endif