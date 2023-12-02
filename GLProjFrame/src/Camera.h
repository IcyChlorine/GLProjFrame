#pragma once
#include "util.h"

#include "Shader.h"
#include "Transform.h"
class Camera :
	public AbsObject
{
protected:
	Transform* transform{ nullptr };
	Transform* proj{ nullptr };
public:
	Camera();
	Camera(AbsObject* father);
	virtual ~Camera();

	virtual void applyCameraTransform(Shader& shader);
};

// for debugging
void print_glm_mat4(std::ostream& out, glm::mat4 mat);
