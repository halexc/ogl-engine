#include "Transform3D.h"

#include "glm/gtx/transform.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/matrix_decompose.hpp"

Transform3D::Transform3D()
{
}

Transform3D::~Transform3D()
{
}

glm::fmat4 Transform3D::getTransform()
{
	if (!valid) {
		validate();
	}
	return transformation;
}

glm::fmat4 Transform3D::getTransformInverted()
{
	if (!valid) {
		validate();
	}
	return inverseT;
}

void Transform3D::setTransform(glm::fmat4 transform)
{
	transformation = transform;
	inverseT = glm::inverse(transform);
	glm::fvec3 dummy3;
	glm::fvec4 dummy4;
	glm::decompose(transformation, size, orientation, position, dummy3, dummy4);
}

void Transform3D::setTransformInverted(glm::fmat4 transform)
{
	inverseT = transform;
	transformation = glm::inverse(transform);
	glm::fvec3 dummy3;
	glm::fvec4 dummy4;
	glm::decompose(transformation, size, orientation, position, dummy3, dummy4);
}

void Transform3D::translateOriented(glm::fvec3 translation)
{
	// To transform locally, the translation has to be rotated according
	// to the orientation, since in the transformation matrix, translation
	// is applied before rotation.
	glm::fquat translationQuat = glm::fquat(0.0f, translation.x, translation.y, translation.z);
	translationQuat = orientation * translationQuat * glm::conjugate(orientation);
	position += glm::fvec3(translationQuat.x, translationQuat.y, translationQuat.z);
	valid = false;
}

void Transform3D::translateOriented(float dx, float dy, float dz)
{
	translateOriented(glm::fvec3(dx, dy, dz));
}

void Transform3D::translate(glm::fvec3 translation)
{
	position += translation;
	valid = false;
}

void Transform3D::translate(float dx, float dy, float dz)
{
	translate(glm::fvec3(dx, dy, dz));
}

void Transform3D::translateGlobal(glm::fvec3 translation)
{
	if (parent == NULL) {
		position += translation;
	}
	else {
		glm::fquat orientationParent = parent->getOrientationGlobal();
		glm::fvec3 sizeParent = parent->getScaleGlobal();
		glm::fquat translationRotated = glm::conjugate(orientationParent) * glm::fquat(0.0f, translation.x, translation.y, translation.z) * orientationParent;
		translation = glm::fvec3(translationRotated.x, translationRotated.y, translationRotated.z) / sizeParent;
		position += translation;
	}
	valid = false;
}

void Transform3D::translateGlobal(float dx, float dy, float dz)
{
	translateGlobal(glm::fvec3(dx, dy, dz));
}

void Transform3D::setPosition(glm::fvec3 position)
{
	this->position = position;
	valid = false;
}

void Transform3D::setPosition(float x, float y, float z)
{
	setPosition(glm::fvec3(x, y, z));
}

glm::fvec3 Transform3D::getPosition()
{
	return position;
}

glm::fvec3 Transform3D::getPositionGlobal()
{
	if (parent) {
		glm::fvec4 globalPos = parent->getTransform() * glm::fvec4(position, 0);
		return glm::fvec3(globalPos.x, globalPos.y, globalPos.z);
	}
	return position;
}

glm::fvec3 Transform3D::getRight()
{
	glm::fquat rightQ(0.0f, 1.0f, 0.0f, 0.0f);
	rightQ = orientation * rightQ * glm::conjugate(orientation);
	return glm::fvec3(rightQ.x, rightQ.y, rightQ.z);
}

glm::fvec3 Transform3D::getUp()
{
	glm::fquat upQ(0.0f, 0.0f, 1.0f, 0.0f);
	upQ = orientation * upQ * glm::conjugate(orientation);
	return glm::fvec3(upQ.x, upQ.y, upQ.z);
}

glm::fvec3 Transform3D::getForward()
{
	glm::fquat fwdQ(0.0f, 0.0f, 0.0f, 1.0f);
	fwdQ = orientation * fwdQ * glm::conjugate(orientation);
	return glm::fvec3(fwdQ.x, fwdQ.y, fwdQ.z);
}

glm::fvec3 Transform3D::getRightGlobal()
{
	glm::fquat orientationGlobal = this->getOrientationGlobal();
	glm::fquat r = orientationGlobal * glm::fquat(0.0f, this->getRight()) * glm::conjugate(orientationGlobal);
	return glm::fvec3(r.x, r.y, r.z);
}

glm::fvec3 Transform3D::getUpGlobal()
{
	glm::fquat orientationGlobal = this->getOrientationGlobal();
	glm::fquat u = orientationGlobal * glm::fquat(0.0f, this->getUp()) * glm::conjugate(orientationGlobal);
	return glm::fvec3(u.x, u.y, u.z);
}

glm::fvec3 Transform3D::getForwardGlobal()
{
	glm::fquat orientationGlobal = this->getOrientationGlobal();
	glm::fquat f = orientationGlobal * glm::fquat(0.0f, this->getForward()) * glm::conjugate(orientationGlobal);
	return glm::fvec3(f.x, f.y, f.z);
}

void Transform3D::rotate(glm::fquat rotation)
{
	orientation = orientation * rotation;
	valid = false;
}

void Transform3D::rotate(float angle, glm::fvec3 axis)
{
	rotate(glm::angleAxis(angle, axis));
}

void Transform3D::rotateAround(glm::fquat rotation, glm::fvec3 center)
{
	glm::fvec3 delta = center;														// Translation to the center
	glm::fquat deltaQuat = glm::fquat(0.0f, delta.x, delta.y, delta.z);				// Conversion to quaternion, so that it can be used for quaternion rotation
	this->rotate(rotation);															// Apply rotation to the transform object
	deltaQuat = -rotation * deltaQuat * glm::conjugate(-rotation);					// Rotate the delta vector.
	glm::fvec3 deltaRotated = glm::fvec3(deltaQuat.x, deltaQuat.y, deltaQuat.z);	// Conversion back to 1x3 - Vector
	this->translate(delta - deltaRotated);											// Apply translation: Move object, to center, then back with the rotation applied

	valid = false;
}

void Transform3D::rotateAround(float angle, glm::fvec3 axis, glm::fvec3 center)
{
	this->rotateAround(glm::angleAxis(angle, axis), center);
}

void Transform3D::rotateAroundGlobal(glm::fquat rotation, glm::fvec3 center)
{
	glm::fvec3 delta = center - this->getPositionGlobal();
	glm::fquat deltaQuat = glm::fquat(0.0f, delta.x, delta.y, delta.z);
	this->rotateGlobal(rotation);
	deltaQuat = -rotation * deltaQuat * glm::conjugate(-rotation);
	glm::fvec3 deltaRotated = glm::fvec3(deltaQuat.x, deltaQuat.y, deltaQuat.z);
	this->translateGlobal(delta - deltaRotated);

	valid = false;
}

void Transform3D::rotateAroundGlobal(float angle, glm::fvec3 axisGlobal, glm::fvec3 center)
{
	this->rotateAroundGlobal(glm::angleAxis(angle, axisGlobal), center);
}

void Transform3D::rotateGlobal(glm::fquat rotation)
{
	if (parent == NULL) {
		orientation = rotation * orientation;
	}
	else {
		glm::fquat parentOrientation = parent->getOrientationGlobal();
		orientation = glm::inverse(parentOrientation) * rotation * parentOrientation * orientation;
	}
	valid = false;
}

void Transform3D::rotateGlobal(float angle, glm::fvec3 axis)
{
	rotateGlobal(glm::angleAxis(angle, axis));
}

void Transform3D::setOrientation(glm::fquat orientation)
{
	this->orientation = orientation;
	valid = false;
}

glm::fquat Transform3D::getOrientation()
{
	return orientation;
}

glm::fquat Transform3D::getOrientationGlobal()
{
	if (parent == NULL)
		return orientation;
	return parent->getOrientationGlobal() * orientation;
}

glm::fvec3 Transform3D::getOrientationEuler()
{
	return glm::eulerAngles(orientation);
}

glm::fvec3 Transform3D::getOrientationGlobalEuler()
{
	if (parent == NULL)
		return glm::eulerAngles(orientation);
	return glm::eulerAngles(parent->getOrientationGlobal() * orientation);
}

void Transform3D::scale(glm::fvec3 scale)
{
	this->scale(scale.x, scale.y, scale.z);
}

void Transform3D::scale(float s)
{
	size *= s;
	valid = false;
}

void Transform3D::scale(float x, float y, float z)
{
	size.x *= x;
	size.y *= y;
	size.z *= z;
	valid = false;
}

void Transform3D::scaleGlobal(glm::fvec3 scale)
{
	// Rotate the scale according to the orientation.
	this->scale(glm::conjugate(orientation) * scale * orientation);
}

void Transform3D::scaleGlobal(float x, float y, float z)
{
	this->scaleGlobal(glm::fvec3(x, y, z));
}

void Transform3D::setScale(glm::fvec3 scale)
{
	this->size = scale;
	valid = false;
}

glm::fvec3 Transform3D::getScale()
{
	return size;
}

glm::fvec3 Transform3D::getScaleGlobal()
{
	if (parent)
		return parent->getScaleGlobal() * size;
	return size;
}

void Transform3D::validate()
{
	// This function is used to not recalculate the transformation matrix with every
	// transformation, but rather whenever it is necessary.
	transformation = glm::scale(glm::transpose(glm::fmat4(	1.0f, 0.0f, 0.0f, position.x,
															0.0f, 1.0f, 0.0f, position.y, 
															0.0f, 0.0f, 1.0f, position.z, 
															0.0f, 0.0f, 0.0f, 1.0f )) * glm::toMat4(orientation), size);
	if (parent) {
		transformation = parent->getTransform() * transformation;
	}
	inverseT = glm::inverse(transformation);
	valid = true;
}
