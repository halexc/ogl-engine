#include "Camera.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/matrix_decompose.hpp"


Camera::Camera()
{
	setProjection(glm::perspective(glm::radians(60.0f), 16.0f / 9.0f, 0.125f, 100.0f));
	lookAt(glm::fvec3(0.0f, 0.5f, 3.0f), glm::fvec3(0.0f, 0.0f, 0.0f), glm::fvec3(0.0f, 1.0f, 0.0f));
}

Camera::Camera(glm::fmat4 projection, glm::fmat4 view)
{
	setProjection(projection);
	setView(view);
}


Camera::~Camera()
{
}

void Camera::setProjection(glm::fmat4 projection)
{
	this->projection = projection;
}

void Camera::setProjection(float rad, float aspectRatio)
{
	projection = glm::perspective(rad, aspectRatio, 0.1f, 100.0f);
}

void Camera::setProjection(float rad, float aspectRatio, float zNear, float zFar)
{
	projection = glm::perspective(rad, aspectRatio, zNear, zFar);
}

glm::fmat4 Camera::getProjectionMatrix()
{
	return projection;
}

void Camera::setView(glm::fmat4 view)
{
	transform.setTransformGlobalInverted(view);
}

void Camera::lookAt(glm::fvec3 eye, glm::fvec3 tgt, glm::fvec3 up)
{
	transform.setTransformGlobalInverted(glm::lookAt(eye, tgt, up));
}

Transform3D * Camera::getTransform()
{
	return &transform;
}
void Camera::translate(glm::fvec3 v)
{
	transform.translate(v);
}
void Camera::translate(float x, float y, float z)
{
	transform.translate(x, y, z);
}
void Camera::setShaderMatrices(Shader * s)
{
	s->use();
	s->setVec3("cameraPos", transform.getPositionGlobal());
	s->setMat4("projection", projection);
	s->setMat4("view", transform.getTransformInverted());
}