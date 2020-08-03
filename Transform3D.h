#pragma once

#include "glm\glm.hpp"
#include "glm\gtc\quaternion.hpp"
#include "glm\gtx\quaternion.hpp"

#include <vector>

class Transform3D
{
public:

	Transform3D();
	~Transform3D();

	// Get the (global) transformation matrix (e.g. to set the model matrix).
	// Usually, the standard transformation matrix is used for objects in the
	// world, while for the view matrix, the inverse camera transformation
	// matrix is more commonly utilised.
	glm::fmat4 getTransform();

	// Get the inverse (global) transformation matrix (e.g. to set the view matrix).
	// Usually, the standard transformation matrix is used for objects in the
	// world, while for the view matrix, the inverse camera transformation
	// matrix is more commonly utilised.
	glm::fmat4 getTransformInverted();

	// Set the local transformation matrix. This also updates the position,
	// orientation, and scale according to the entries in the transformation
	// matrix. 
	// Pass the transformation matrix as a glm::fmat4.
	void setTransform(glm::fmat4 transform);

	// Set the global transformation matrix. This also updates the position,
	// orientation, and scale according to the entries in the transformation
	// matrix.
	// Pass the transformation matrix as a glm::fmat4.
	void setTransformGlobal(glm::fmat4 transform);

	// Set the local transformation matrix, but in inverted form. This also 
	// updates the position, orientation, and scale according to the entries 
	// in the transformation matrix.
	// Pass the inverted transformation matrix as a glm::fmat4.
	void setTransformInverted(glm::fmat4 transform);

	// Set the global transformation matrix, but in inverted form. This also 
	// updates the position, orientation, and scale according to the entries 
	// in the transformation matrix.
	// Pass the inverted transformation matrix as a glm::fmat4.
	void setTransformGlobalInverted(glm::fmat4 transform);

	//Translation:

	// Apply a translation. However, instead of translating in the Transform3D's
	// local space, the translation is applied considering its orientation, e.g.
	// the transformation is applied in what would be its childrens' local space.
	// Pass the translation as a glm::fvec3.
	void translateOriented(glm::fvec3 translation);

	// Apply a translation. However, instead of translating in the Transform3D's
	// local space, the translation is applied considering its orientation, e.g.
	// the transformation is applied in what would be its childrens' local space.
	// Pass the translation as three seperate float values.
	void translateOriented(float dx, float dy, float dz);

	// Apply a translation in the Transform3D's local space.
	// Pass the translation as a glm::fvec3.
	void translate(glm::fvec3 translation);

	// Apply a translation in the Transform3D's local space.
	// Pass the translation as three seperate float values.
	void translate(float dx, float dy, float dz);

	// Apply a translation in the global space.
	// Pass the translation as a glm::fvec3.
	void translateGlobal(glm::fvec3 translation);

	// Apply a translation in the global space.
	// Pass the translation as three seperate float values.
	void translateGlobal(float dx, float dy, float dz);

	// Set the Transform3D's position in local space.
	// Pass the position as a glm::fvec3.
	void setPosition(glm::fvec3 position);

	// Set the Transform3D's position in local space.
	// Pass the position as three seperate float values.
	void setPosition(float x, float y, float z);

	// Get the Transform3D's position in local space.
	// The return value is of type glm::fvec3.
	glm::fvec3 getPosition();

	// Get the Transform3D's position in global space.
	// The return value is of type glm::fvec3.
	glm::fvec3 getPositionGlobal();

	// Get a vector pointing to the right of the transform, considering
	// its orientation, in its local space. This is equivalent to the x-axis 
	// of its children's local space.
	// The return value is of type glm::fvec3.
	glm::fvec3 getRight();

	// Get a vector pointing upwards of the transform, considering its
	// orientation, in its local space. This is equivalent to the y-axis 
	// of its children's local space.
	// The return value is of type glm::fvec3.
	glm::fvec3 getUp();

	// Get a vector pointing forwards from the transform, considering
	// its orientation, in its local space. This is equivalent to the z-axis 
	// of its children's local space.
	// The return value is of type glm::fvec3.
	glm::fvec3 getForward();

	// Get a vector pointing to the right of the transform, considering
	// its orientation, in its global space. This is equivalent to the x-axis 
	// of its children's local space, but now in the global coordinate system.
	// The return value is of type glm::fvec3.
	glm::fvec3 getRightGlobal();

	// Get a vector pointing upwards of the transform, considering its
	// orientation, in its global space. This is equivalent to the y-axis 
	// of its children's local space, but now in the global coordinate system.
	// The return value is of type glm::fvec3.
	glm::fvec3 getUpGlobal();

	// Get a vector pointing forwards from the transform, considering
	// its orientation, in its global space. This is equivalent to the z-axis 
	// of its children's local space, but now in the global coordinate system.
	// The return value is of type glm::fvec3.
	glm::fvec3 getForwardGlobal();

	//Rotation:

	// Apply a rotation to the Transform3D, altering its current orientation,
	// using a quaternion.
	// Pass the rotation as a glm::fquat.
	void rotate(glm::fquat rotation);

	// Apply a rotation to the Transform3D, altering its current orientation,
	// around a local axis.
	// Pass the rotation angle as a float and the local axis as a glm::fvec3.
	void rotate(float angle, glm::fvec3 axis);

	// Rotate the Transform3D around a point in its local space. The parameters
	// are the rotation as a quaternion (glm::fquat) and the center of the rotation
	// in local space as a vector (glm::fvec3).
	// Pass the rotation as a glm::fquat, and optionally the local center of  
	// rotation as glm::fvec3.
	void rotateAround(glm::fquat rotation, glm::fvec3 delta = glm::fvec3(0.0f));

	// Rotate the Transform3D around a point in its local space. The parameters
	// are the rotation, consisting of angle (float) and axis (glm::fvec3), and 
	// the center of the rotation in local space as a vector (glm::fvec3).
	// Pass the rotation angle as a float, the axis of rotation as a glm::fvec3, 
	// and optionally the local center of rotation as glm::fvec3.
	void rotateAround(float angle, glm::fvec3 axis, glm::fvec3 delta = glm::fvec3(0.0f));

	// Rotate the Transform3D around a point in its global space. The parameters
	// are the rotation as a quaternion (glm::fquat) and the center of the rotation
	// in global space as a vector (glm::fvec3).
	// Pass the rotation as a glm::fquat, and optionally the global center of 
	// rotation as glm::fvec3.
	void rotateAroundGlobal(glm::fquat rotation, glm::fvec3 center = glm::fvec3(0.0f));

	// Rotate the Transform3D around a point in its global space. The parameters
	// are the rotation, consisting of angle (float) and axis (glm::fvec3), and 
	// the center of the rotation in global space as a vector (glm::fvec3).
	// Pass the rotation angle as a float, the axis of rotation as a glm::fvec3, 
	// and optionally the global center of rotation as glm::fvec3.
	void rotateAroundGlobal(float angle, glm::fvec3 axisGlobal, glm::fvec3 center = glm::fvec3(0.0f));

	// Apply a rotation to the Transform3D, altering its current orientation,
	// using a quaternion, this time representing a rotation in global space.
	// Pass the global rotation as a glm::fquat.
	void rotateGlobal(glm::fquat rotation);

	// Apply a rotation to the Transform3D, altering its current orientation,
	// around a global axis.
	// Pass the rotation angle as a float and the global axis as a glm::fvec3.
	void rotateGlobal(float angle, glm::fvec3 axis);

	// Set the orientation of the Transform3D, as a quaternion, in its local
	// space.
	// Pass the orientation as a glm::fquat.
	void setOrientation(glm::fquat orientation);

	// Set the orientation of the Transform3D, using euler angles, in its local
	// space.
	// Pass the orientation as a glm::fvec3 containing the euler angles.
	void setOrientation(glm::fvec3 eulerAngle);

	// Set the orientation of the Transform3D, using euler angles, in its local
	// space.
	// Pass the orientation as three seperate float values, each representing the
	// euler rotation around the corresponding axis.
	void setOrientation(float rotX, float rotY, float rotZ);

	// Get the local orientation of the Transform3D as a quaternion.
	// The return value is of type glm::fquat.
	glm::fquat getOrientation();


	// Get the global orientation of the Transform3D as a quaternion.
	// The return value is of type glm::fquat.
	glm::fquat getOrientationGlobal();

	// Get the local orientation of the Transform3D in terms of euler angles.
	// The return value is a vector of type glm::fvec3.
	glm::fvec3 getOrientationEuler();

	// Get the global orientation of the Transform3D in terms of euler angles.
	// The return value is a vector of type glm::fvec3.
	glm::fvec3 getOrientationGlobalEuler();

	//Scaling:

	// Scale the Transform3D along its oriented x, y, and z axis, using a vector.
	// Pass the scaling factors as a glm::fvec3.
	void scale(glm::fvec3 scale);

	// Scale the Transform3D equally along all axises.
	// Pass the scaling factor as a float.
	void scale(float s);

	// Scale the Transform3D along its oriented x, y, and z axis.
	// Pass the scaling factors as three seperate float values.
	void scale(float x, float y, float z);

	// Scale the Transform3D along the global x, y, and z axis, using a vector.
	// Pass the scaling factors as a glm::fvec3.
	void scaleGlobal(glm::fvec3 scale);

	// Scale the Transform3D along the global x, y, and z axis.
	// Pass the scaling factors as three seperate float values.
	void scaleGlobal(float x, float y, float z);

	// Set the size of a Transform3D, using a vector representing the scale across
	// all three oriented axises.
	// Pass the optional size as a glm::fvec3.
	void setScale(glm::fvec3 scale = glm::fvec3(1.0f, 1.0f, 1.0f));

	// Set the size of a Transform3D with three floats representing the scale across
	// all three oriented axises.
	// Pass three floats for the respective axises.
	void setScale(float x, float y, float z);

	// Get the oriented scale of the Transform3D as a vector, with each component
	// representing the scale factor along the respective axis.
	// The return value is a vector of type glm::fvec3.
	glm::fvec3 getScale();

	// Get the global scale of the Transform3D as a vector, with each component
	// representing the scale factor along the respective axis.
	// The return value is a vector of type glm::fvec3.
	glm::fvec3 getScaleGlobal();

	// Invalidate the Transform3D's current transformation matrices. This function
	// is called whenever there is a transformation or a parent has been altered.
	// After calling this function, the transformation matrix and its inverse are
	// recalculated on the next call of getTransform() or getTransformInverted().
	void invalidate();

	// Set the Transform3D's parent. If the global position, orientation, and scale
	// should be kept, set keepGlobalTF = true. If instead the Transform3D's relative
	// transformation to its parent object should stay constant, set keepGlobalTF = false.
	// If the Transform3D was previously a child of another instance, this is correctly
	// updated.
	// Pass a pointer to a parent object (can be NULL), and optionally a boolean whether
	// the global transformation should be kept.
	void setParent(Transform3D * parent, bool keepGlobalTF = true);

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

