#pragma once

#ifndef SHADER_H
#define SHADER_H

/*
	Shader class from https://learnopengl.com/Getting-started/Shaders
	Modified by StarSky@PKU
*/
#include <glad/glad.h> // include glad to get all the required OpenGL headers

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;

#include "util.h"

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
	void setUniform(const std::string &name, bool value) const;
	void setUniform(const std::string &name, int value) const;
	void setUniform(const std::string &name, float value) const;
};

#endif