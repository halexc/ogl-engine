#pragma once

#include "glm\glm.hpp"
#include <vector>

#define INERTIA_TENSOR_SPHERE			0x00
#define INERTIA_TENSOR_ELLIPSOID		0x01
#define INERTIA_TENSOR_CUBOID			0x02
#define INERTIA_TENSOR_ROD_CENTER		0x03
#define INERTIA_TENSOR_ROD_END			0x04
#define INERTIA_TENSOR_CYLINDER			0x05
#define INERTIA_TENSOR_CYLINDER_HOLLOW	0x06

//Struct to apply forces over time:
struct forceOverTime {
	glm::fvec3 force;
	double timeRemaining;
};

// Rigidbody object for physics calculations:
class Rigidbody {
public:
	float mass;
	glm::fmat3 inertiaTensor = glm::fmat3(1.0f);
	glm::fvec3 momentumLinear;
	glm::fvec3 momentumAngular;

	float friction = 0.125f;

	// Accelerate the rigidbody.
	void applyAcceleration(glm::fvec3 acc);
	// Continuously accelerate the rigidbody over time.
	void applyAccelerationOverTime(glm::fvec3 acc, double time);

	void applyAngularAcceleration(glm::fvec3 acc);

	virtual void update(double delta);

	// Apply a force to the rigidbody, optionally with leverage, within the local coordinate system of the object.
	void applyForce(glm::fvec3 F, glm::fvec3 leverage = glm::fvec3(0.0f, 0.0f, 0.0f));
	// Continuously apply a force to the rigidbody, optionally with leverage, within the local coordinate system of the object.
	void applyForceOverTime(glm::fvec3 F, double time, glm::fvec3 leverage = glm::fvec3(0.0f, 0.0f, 0.0f));

	void generateInertiaTensor(char type, float width_or_radius, float height = 0, float depth_or_cutout = 0, float mass = 0);
	void generateInertiaTensor_Sphere(float r, float mass = 0);
	void generateInertiaTensor_Ellipsoid(float semiA, float semiB, float semiC, float mass = 0);
	void generateInertiaTensor_Cuboid(float a, float b, float c, float mass = 0);
	void generateInertiaTensor_RodAboutCenter(float length, float mass = 0);
	void generateInertiaTensor_RodAboutEnd(float length, float mass = 0);
	void generateInertiaTensor_Cylinder(float r, float height, float r_hollow = 0, float mass = 0);

private:
	std::vector<forceOverTime> continuousForces;
};