#include "Transform.h"

Transform::Transform()
{
}


Transform::~Transform()
{
}

void Transform::apply(Shader& shader, string uniVarName)
{
	if (!enabled) return;

	unsigned int transformLoc = glGetUniformLocation(shader.m_shaderID, uniVarName.c_str());
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transMat));
}
