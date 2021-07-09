#pragma once
#include "common_t1.h"

#include "Shader.h"

class Transform:public AbsObject
{
protected:
	//�任����
	glm::mat4 transMat{ glm::mat4(1.0f) };
public:
	Transform();
	Transform(AbsObject* father) :AbsObject{ father } {}
	~Transform();

	bool enabled{ false };

	glm::mat4 getTransformMat() { return transMat; }
	void setTransformMat(const glm::mat4& mat) { transMat = mat; }
	void apply(Shader& shader, string uniVarName = "transform");//uniVarName:��Shader�б任����ı�����
	void enable(bool enabled) { this->enabled = enabled; }
	void reset() { this->transMat = glm::mat4(1.0f); }
};

