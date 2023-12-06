#pragma once
#include "util.h"

#include "glm/mat4x4.hpp"

#include "Shader.h"
class Camera :
	public AbsObject
{
protected:
	glm::mat4 view{ glm::mat4(1.0f) };
	glm::mat4 proj{ glm::mat4(1.0f) };
public:
	Camera();
	Camera(AbsObject* father);
	virtual ~Camera();

	virtual void applyCameraTransform(Shader& shader);
};

// for debugging
void print_glm_mat4(std::ostream& out, glm::mat4 mat);
