#include "Shader.h"

Shader::Shader(const char * vertSrcPath, const char * fragSrcPath)
{
	// 1. retrieve the vertex/fragment source code from filePath
	string vertSrc, fragSrc;
	ifstream vertShaderFile, fragShaderFile;
	// ensure ifstream objects can throw exceptions:
	vertShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fragShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		// open files
		vertShaderFile.open(vertSrcPath);
		fragShaderFile.open(fragSrcPath);
		stringstream ssVertShader, ssFragShader;
		// read file's buffer contents into streams
		ssVertShader << vertShaderFile.rdbuf();
		ssFragShader << fragShaderFile.rdbuf();
		// close file handlers
		vertShaderFile.close();
		fragShaderFile.close();
		// convert stream into string
		vertSrc = ssVertShader.str();
		fragSrc = ssFragShader.str();
	}
	catch (ifstream::failure e)
	{
		errorf("Fail to load shader src from file `%s`, `%s`.\n", vertSrcPath, fragSrcPath);
		throw exception();
	}
	const char* pVertSrc = vertSrc.c_str();
	const char* pFragSrc = fragSrc.c_str();

	// 2. compile shaders via drivers provided to ogl
	unsigned int vertShaderID, fragShaderID;
	int success;
	char infoLog[512];
	// vertex Shader
	vertShaderID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertShaderID, 1, &pVertSrc, NULL);
	glCompileShader(vertShaderID);
	// print compile errors if any
	glGetShaderiv(vertShaderID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertShaderID, 512, NULL, infoLog);
		errorf("Fail to compile vertex shader, error info is as below:\n%s\n", infoLog);
		throw exception();
	};
	// fragment Shader
	fragShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragShaderID, 1, &pFragSrc, NULL);
	glCompileShader(fragShaderID);
	// print compile errors if any
	glGetShaderiv(fragShaderID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragShaderID, 512, NULL, infoLog);
		errorf("Fail to compile fragment shader, error info is as below:\n%s\n", infoLog);
		throw exception();
	};

	// shader Program
	m_shaderID = glCreateProgram();
	glAttachShader(m_shaderID, vertShaderID);
	glAttachShader(m_shaderID, fragShaderID);
	glLinkProgram(m_shaderID);
	// print linking errors if any
	glGetProgramiv(m_shaderID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(m_shaderID, 512, NULL, infoLog);
		errorf("Fail to link vert&frag shaders\n%s\n", infoLog);
		throw exception();
	}

	// delete the shaders as they're linked into our program now and no longer necessery
	glDeleteShader(vertShaderID);
	glDeleteShader(fragShaderID);
}
void Shader::use()
{
	glUseProgram(m_shaderID);
}
void Shader::setUniform(const string &varName, bool value)
{
	auto uniform_loc = glGetUniformLocation(m_shaderID, varName.c_str());
	glUniform1i(uniform_loc, (int)value);
}
void Shader::setUniform(const string &varName, int value)
{
	auto uniform_loc = glGetUniformLocation(m_shaderID, varName.c_str());
	glUniform1i(uniform_loc, value);
}
void Shader::setUniform(const string &varName, float value)
{
	auto uniform_loc = glGetUniformLocation(m_shaderID, varName.c_str());
	glUniform1f(uniform_loc, value);
}
void Shader::setUniform(const string &varName, const glm::vec2 &value)
{
	auto uniform_loc = glGetUniformLocation(m_shaderID, varName.c_str());
	glUniform2fv(uniform_loc, 1, &value[0]);
}
void Shader::setUniform(const string &varName, const glm::vec3 &value)
{
	auto uniform_loc = glGetUniformLocation(m_shaderID, varName.c_str());
	glUniform3fv(uniform_loc, 1, &value[0]);
}
void Shader::setUniform(const string &varName, const glm::vec4 &value)
{
	auto uniform_loc = glGetUniformLocation(m_shaderID, varName.c_str());
	glUniform4fv(uniform_loc, 1, &value[0]);
}


void Shader::setUniformMatrix(const string & varName, const glm::mat4& mat) {
	auto uniform_loc = glGetUniformLocation(m_shaderID, varName.c_str());
	glUniformMatrix4fv(uniform_loc, 1, GL_FALSE, glm::value_ptr(mat));
}
void Shader::setUniformMatrix(const string & varName, const glm::mat3& mat) {
	auto uniform_loc = glGetUniformLocation(m_shaderID, varName.c_str());
	glUniformMatrix3fv(uniform_loc, 1, GL_FALSE, glm::value_ptr(mat));
}