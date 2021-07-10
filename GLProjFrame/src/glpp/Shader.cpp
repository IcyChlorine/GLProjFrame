#include "glpp/Shader.h"

Shader::Shader(const char * vertSrcPath, const char * fragSrcPath)
{
	
	char buf[512];
	sprintf_s(buf, 512u, "Loading shader from path %s, %s...",vertSrcPath, fragSrcPath);
	Logging::info(buf);
	// 1. retrieve the vertex/fragment source code from filePath
	string vertSrc;
	string fragSrc;
	ifstream vertShaderFile;
	ifstream fragShaderFile;
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
	catch (std::ifstream::failure e)
	{
		Logging::error("Fail to load shader src from file.");
		//cout << "Error: Fail to load shader src from file" << std::endl;
		return;
	}
	if(vertSrc=="" || fragSrc==""){
		Logging::error("Fail to load shader src from file.");
		return;
	}
	const char* pVertSrc = vertSrc.c_str();
	const char* pFragSrc = fragSrc.c_str();

	// 2. compile shaders
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
		sprintf_s(buf,512u,"Fail to compile vertex shader, with internal error msg:\n\t%s",infoLog);
		//std::cout << "Error: Fail to compile vertex shader\n" << infoLog << std::endl;
		Logging::error(buf);
		return;
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
		//std::cout << "Error: Fail to compile fragment shader\n" << infoLog << std::endl;
		sprintf_s(buf,512u,"Fail to compile fragment shader, with internal error msg:\n\t%s",infoLog);
		Logging::error(buf);
		return;
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
		//std::cout << "Error: Fail to link vert&frag shaders\n" << infoLog << std::endl;
		sprintf_s(buf,512u,"Fail to link vert&frag shaders, with internal error msg:\n\t%s",infoLog);
		Logging::error(buf);
	}

	// delete the shaders as they're linked into our program now and no longer necessery
	glDeleteShader(vertShaderID);
	glDeleteShader(fragShaderID);
}
void Shader::use()
{
	glUseProgram(m_shaderID);
}
void Shader::setUniform(const string &varName, bool value) const
{
	glUniform1i(glGetUniformLocation(m_shaderID, varName.c_str()), (int)value);
}
void Shader::setUniform(const string &varName, int value) const
{
	glUniform1i(glGetUniformLocation(m_shaderID, varName.c_str()), value);
}
void Shader::setUniform(const string &varName, float value) const
{
	glUniform1f(glGetUniformLocation(m_shaderID, varName.c_str()), value);
}
