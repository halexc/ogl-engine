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
	glm::fvec3 getLookDir(glm::fvec3 up = glm::fvec3(0.0f, 1.0f, 0.0f));

	void translate(glm::fvec3 v);
	void translate(float x, float y, float z);

	void setShaderMatrices(Shader * s);

private:
	Transform3D transform;
	glm::fmat4 projection;
};

