#pragma once

#include <glm\glm.hpp>

#include "Transform3D.h"
#include "Shader.h"

class Camera
{
public:
	Camera();
	Camera(glm::fmat4 projection, glm::fmat4 view = glm::fmat4(1.0f));
	~Camera();

	void setProjection(glm::fmat4 projection);
	void setProjection(float rad, float aspectRatio);
	void setProjection(float rad, float aspectRatio, float zNear, float zFar);
	glm::fmat4 getProjectionMatrix();

	void setView(glm::fmat4 view);
	void lookAt(glm::fvec3 eye, glm::fvec3 tgt, glm::fvec3 up);
	Transform3D * getTransform();

	void translate(glm::fvec3 v);
	void translate(float x, float y, float z);
	/*
	void setView(glm::fvec3 position, glm::fvec3 target, glm::fvec3 upVector);
	glm::fmat4 getViewMatrix();

	void setPosition(glm::fvec3 position);
	void setPosition(float x, float y, float z);
	glm::fvec3 getPosition();


	void setRotation(glm::fquat rotation);
	glm::fquat getRotation();

	void rotate(glm::fquat rotation);
	void rotate(float angle, glm::fvec3 axis);
	void rotateAroundPoint(glm::fquat rotation, glm::fvec3 center);
	void rotateAroundPoint(float angle, glm::fvec3 axis, glm::fvec3 center);

	*/
	void setViewProjection(Shader &s);

private:
	Transform3D transform;
	glm::fmat4 projection;
};

