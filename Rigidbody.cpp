#include "Rigidbody.h"

void Rigidbody::applyAcceleration(glm::fvec3 acc)
{
	momentumLinear += acc;
}

void Rigidbody::applyAngularAcceleration(glm::fvec3 acc)
{
	momentumAngular += acc;
}

void Rigidbody::applyForce(glm::fvec3 F, glm::fvec3 leverage)
{
	applyAcceleration((1 / mass) * F);
	glm::fvec3 torque = glm::cross(F, leverage);
	applyAngularAcceleration(glm::inverse(inertiaTensor) * torque);
}

void Rigidbody::generateInertiaTensor(char type, float width_or_radius, float height, float depth_or_cutout, float mass)
{
	if (type == INERTIA_TENSOR_SPHERE) {
		generateInertiaTensor_Sphere(width_or_radius, mass);
	}
	if (type == INERTIA_TENSOR_ELLIPSOID) {
		generateInertiaTensor_Ellipsoid(width_or_radius, height, depth_or_cutout, mass);
	}
	if (type == INERTIA_TENSOR_CUBOID) {
		generateInertiaTensor_Ellipsoid(width_or_radius, height, depth_or_cutout, mass);
	}
	if (type == INERTIA_TENSOR_ROD_CENTER) {
		generateInertiaTensor_RodAboutCenter(width_or_radius, mass);
	}
	if (type == INERTIA_TENSOR_ROD_END) {
		generateInertiaTensor_RodAboutEnd(width_or_radius, mass);
	}
	if (type == INERTIA_TENSOR_CYLINDER) {
		generateInertiaTensor_Cylinder(width_or_radius, height, depth_or_cutout, mass);
	}
}

void Rigidbody::generateInertiaTensor_Sphere(float r, float mass)
{
	if (mass <= 0) mass = this->mass;
	inertiaTensor = glm::fmat3(mass * (2 / 5) * r*r);
}

void Rigidbody::generateInertiaTensor_Ellipsoid(float semiA, float semiB, float semiC, float mass)
{
	if (mass <= 0) mass = this->mass;
	float m = mass * (1 / 5);
	inertiaTensor = glm::fmat3(0.0f);
	inertiaTensor[0][0] = m * (semiB*semiB + semiC*semiC);
	inertiaTensor[1][1] = m * (semiA*semiA + semiC*semiC);
	inertiaTensor[2][2] = m * (semiA*semiA + semiB*semiB);
}

void Rigidbody::generateInertiaTensor_Cuboid(float a, float b, float c, float mass)
{
	if (mass <= 0) mass = this->mass;
	float m = mass * (1 / 12);
	inertiaTensor = glm::fmat3(0.0f);
	inertiaTensor[0][0] = m * (b*b + c*c);
	inertiaTensor[1][1] = m * (a*a + c*c);
	inertiaTensor[2][2] = m * (a*a + b*b);
}

void Rigidbody::generateInertiaTensor_RodAboutCenter(float length, float mass)
{
	if (mass <= 0) mass = this->mass;
	float m = mass * (1 / 12);
	inertiaTensor = glm::fmat3(0.0f);
	inertiaTensor[0][0] = m * (length*length);
	inertiaTensor[2][2] = m * (length*length);
}

void Rigidbody::generateInertiaTensor_RodAboutEnd(float length, float mass)
{
	if (mass <= 0) mass = this->mass;
	float m = mass * (1 / 3);
	inertiaTensor = glm::fmat3(0.0f);
	inertiaTensor[0][0] = m * (length*length);
	inertiaTensor[2][2] = m * (length*length);
}

void Rigidbody::generateInertiaTensor_Cylinder(float r, float height, float r_hollow, float mass)
{
	if (mass <= 0) mass = this->mass;
	float m = mass * (1 / 12);
	inertiaTensor = glm::fmat3(0.0f);
	inertiaTensor[0][0] = m * (3 * (r*r - r_hollow*r_hollow) + height*height);
	inertiaTensor[1][1] = m * (3 * (r*r + r_hollow*r_hollow) + height*height);
	inertiaTensor[2][2] = m * (6 * (r*r + r_hollow*r_hollow));
}
