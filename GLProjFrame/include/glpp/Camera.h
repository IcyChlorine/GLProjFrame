#pragma once
#include "util.h"

#include "Shader.h"
#include "Transform.h"
class Camera :
	public AbsObject
{
protected:
	Transform* transform{ nullptr };
public:
	Camera();
	Camera(AbsObject* father);
	virtual ~Camera();

	inline void setTransformMat(const glm::mat4& transMat)
		{transform->setTransformMat(transMat);}
	inline glm::mat4 getTransformMat(){ return transform->getTransformMat(); }

	virtual void applyCameraTransform(Shader& shader);
};

