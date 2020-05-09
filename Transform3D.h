#pragma once

#include <glm\glm.hpp>
#include <glm\gtc\quaternion.hpp>
#include <glm\gtx\quaternion.hpp>

#include <vector>

class Transform3D
{
public:

	Transform3D();
	~Transform3D();

	// Get the transformation matrix (e.g. to set the model matrix).
	// Usually, the standard transformation matrix is used for objects in the
	// world, while for the view matrix, the inverse camera transformation
	// matrix is more commonly utilised.
	glm::fmat4 getTransform();

	// Get the inverse transformation matrix (e.g. to set the view matrix).
	// Usually, the standard transformation matrix is used for objects in the
	// world, while for the view matrix, the inverse camera transformation
	// matrix is more commonly utilised.
	glm::fmat4 getTransformInverted();

	void setTransform(glm::fmat4 transform);
	void setTransformGlobal(glm::fmat4 transform);

	void setTransformInverted(glm::fmat4 transform);
	void setTransformGlobalInverted(glm::fmat4 transform);

	//Translation:
	void translateOriented(glm::fvec3 translation);
	void translateOriented(float dx, float dy, float dz);

	void translate(glm::fvec3 translation);
	void translate(float dx, float dy, float dz);

	void translateGlobal(glm::fvec3 translation);
	void translateGlobal(float dx, float dy, float dz);

	void setPosition(glm::fvec3 position);
	void setPosition(float x, float y, float z);

	virtual void update(double delta);

	glm::fvec3 getPosition();
	glm::fvec3 getPositionGlobal();

	glm::fvec3 getRight();
	glm::fvec3 getUp();
	glm::fvec3 getForward();

	glm::fvec3 getRightGlobal();
	glm::fvec3 getUpGlobal();
	glm::fvec3 getForwardGlobal();

	//Rotation:
	void rotate(glm::fquat rotation);
	void rotate(float angle, glm::fvec3 axis);

	void rotateAround(glm::fquat rotation, glm::fvec3 center = glm::fvec3(0.0f));
	void rotateAround(float angle, glm::fvec3 axis, glm::fvec3 center = glm::fvec3(0.0f));

	void rotateAroundGlobal(glm::fquat rotation, glm::fvec3 center = glm::fvec3(0.0f));
	void rotateAroundGlobal(float angle, glm::fvec3 axisGlobal, glm::fvec3 center = glm::fvec3(0.0f));

	void rotateGlobal(glm::fquat rotation);
	void rotateGlobal(float angle, glm::fvec3 axis);

	void setOrientation(glm::fquat orientation);
	glm::fquat getOrientation();
	glm::fquat getOrientationGlobal();
	glm::fvec3 getOrientationEuler();
	glm::fvec3 getOrientationGlobalEuler();

	//Scaling;
	void scale(glm::fvec3 scale);
	void scale(float s);
	void scale(float x, float y, float z);

	void scaleGlobal(glm::fvec3 scale);
	void scaleGlobal(float x, float y, float z);

	void setScale(glm::fvec3 scale);

	glm::fvec3 getScale();
	glm::fvec3 getScaleGlobal();

	void invalidate();

	void setParent(Transform3D * parent);

private:
	//Does this transform have a parent?
	Transform3D * parent = NULL;
	std::vector<Transform3D *> children;

	// Update the transformation matrices to align with the stored transformation data
	void validate();
	// Are the current transformation matrices still valid?
	bool valid = true;

	glm::fmat4 transformation = glm::fmat4(1.0f);
	glm::fmat4 inverseT = glm::fmat4(1.0f);

	glm::fvec3 position = glm::fvec3(0.0f, 0.0f, 0.0f);
	glm::fquat orientation = glm::fquat(1.0f, 0.0f, 0.0f, 0.0f);
	glm::fvec3 size = glm::fvec3(1.0f, 1.0f, 1.0f);
};

