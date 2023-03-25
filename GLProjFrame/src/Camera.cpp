#include "Camera.h"

Camera::Camera()
{
	this->transform = new Transform();
	this->proj = new Transform();
}

Camera::Camera(AbsObject * father):
	AbsObject{father}
{
	this->transform = new Transform();
	this->proj = new Transform();
}


Camera::~Camera()
{
	delete this->transform;
	delete this->proj;
}

void Camera::applyCameraTransform(Shader& shader)
{
	shader.setUniformMatrix("transform", this->transform->getTransformMat());
	shader.setUniformMatrix("proj", this->proj->getTransformMat());
}
