#include "Rigidbody.h"


void Rigidbody::update(double delta)
{
	// Update position and rotation:
	getTransform()->translateGlobal(float(delta) * speedLinear);
	getTransform()->rotateGlobal(float(delta) * glm::length(speedAngular), speedAngular);

	// Iterate through all active forces and apply them
	std::vector<ContinuousForce>::iterator it = continuousForces.begin();
	while (it != continuousForces.end()) {
		double time = fmin(delta, it->timeRemaining);

		applyForceOverPast(it->force, time, it->leverage);

		it->timeRemaining -= time;
		if (it->timeRemaining <= 0.0f) {
			it = continuousForces.erase(it);
		}
		else it++;
	}
}

void Rigidbody::applyAcceleration(glm::fvec3 acc)
{
	speedLinear += acc;
}

void Rigidbody::applyAccelerationOverTime(glm::fvec3 acc, double time)
{
	ContinuousForce force = ContinuousForce();
	force.force = mass * acc;
	force.leverage = glm::fvec3(0.0f, 0.0f, 0.0f);
	continuousForces.push_back(force);
}

void Rigidbody::applyAngularAcceleration(glm::fvec3 acc)
{
	speedAngular += acc;
}

void Rigidbody::applyAngularAccelerationOverTime(glm::fvec3 acc, double time)
{
	ContinuousForce force = ContinuousForce();
	force.force = inertiaTensor * acc / 2.0f;
	force.leverage = glm::fvec3(1.0f, 0.0f, 0.0f);
	force.timeRemaining = time;
	continuousForces.push_back(force);
	force.force = -force.force;
	force.leverage = glm::fvec3(-1.0f, 0.0f, 0.0f);
	continuousForces.push_back(force);
}


void Rigidbody::applyForce(glm::fvec3 F, glm::fvec3 leverage)
{
	applyAcceleration((1 / mass) * F);
	glm::fvec3 torque = glm::cross(F, leverage);
	applyAngularAcceleration(glm::inverse(inertiaTensor) * torque);
}

void Rigidbody::applyForceOverTime(glm::fvec3 F, double time, glm::fvec3 leverage)
{
	ContinuousForce force = ContinuousForce();
	force.force = F;
	force.leverage = leverage;
	force.timeRemaining = time;
	continuousForces.push_back(force);
}

void Rigidbody::applyForceOverPast(glm::fvec3 F, double time, glm::fvec3 leverage)
{
	// Apply Force for future calculations:
	applyForce(F * float(time), leverage);

	// Retrospectively update the transform, assuming linear acceleration (a/2 * t^2):
	getTransform()->translateGlobal(0.5f * F * float(time * time));
	glm::fvec3 angular = 0.5f * glm::cross(F, leverage) * float(time * time);
	getTransform()->rotateGlobal(glm::length(angular), angular);
}

void Rigidbody::reflect(glm::fvec3 normal, float damping, float rotImpact, glm::fvec3 rotLeverage)
{
	bool applyRot = (rotImpact != 0 && rotLeverage.length() > 0);
	damping = fmaxf(0.0f, 1.0f - damping);

	normal = glm::normalize(normal);

	// Energy before reflection
	float E_kin1 = 0.5f * this->mass * glm::length2(this->speedLinear);
	float E_rot1 = 0.5f * glm::length(this->inertiaTensor * (this->speedAngular * this->speedAngular));

	// Add rotation portion
	if (applyRot) this->speedLinear += rotImpact * glm::cross(this->speedAngular, rotLeverage);
	// Reflect the velocity
	this->speedLinear = this->speedLinear - 2.0f * glm::dot(this->speedLinear, normal) * normal;

	// Adjust rotation via energy conservation
	float E_kin2 = 1 / 2 * this->mass * glm::length2(this->speedLinear);
	float p = (E_kin1 + E_rot1 - E_kin2) / E_rot1; // E_rot2 = p * E_rot1 => sqrt(p) * w_1 = w_2
	this->speedAngular *= sqrtf(p);

	// Apply damping:
	this->speedAngular *= damping;
	this->speedLinear *= damping;
}

void Rigidbody::applyFriction(glm::fvec3 normalForce, glm::fvec3 leverage, double delta)
{
	glm::fvec3 speedRot = glm::cross(speedAngular, leverage);

	// Friction versus "sliding"
	if (delta > 0) applyForceOverPast(friction * normalForce.length() * -glm::normalize(speedLinear + speedRot), delta, leverage);
	else applyForce(friction * normalForce.length() * -glm::normalize(speedLinear + speedRot), leverage);

	// Rolling resistance
	if (delta > 0) applyForceOverPast(rollResistance * normalForce.length() * -glm::normalize(speedRot), delta, leverage);
	else applyForce(rollResistance * normalForce.length() * -glm::normalize(speedRot), leverage);
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
