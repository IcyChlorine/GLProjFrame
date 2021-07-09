#include "glpp/Camera.h"



Camera::Camera()
{
	this->transform = new Transform();
	this->transform->enable(true);
}

Camera::Camera(AbsObject * father):
	AbsObject{father}
{
	this->transform = new Transform();
	this->transform->enable(true);
}


Camera::~Camera()
{
	delete this->transform;
}

void Camera::applyCameraTransform(Shader& shader)
{
	this->transform->apply(shader);
}
