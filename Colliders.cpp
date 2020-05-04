#include "Colliders.h"

#include <glm\gtc\quaternion.hpp>
#include <glm\gtx\quaternion.hpp>

float sign(float f) {
	return -1.0f + 2.0f * (f >= 0);
}

Ray::Ray()
{
	origin = glm::fvec3(0.0f, 0.0f, 0.0f);
	direction = glm::fvec3(0.0f, 0.0f, 1.0f);
}

Ray::Ray(glm::fvec3 origin, glm::fvec3 dir)
{
	this->origin = origin;
	this->direction = glm::normalize(dir);
}


Ray::~Ray()
{
}

void Ray::setOrigin(glm::fvec3 origin)
{
	this->origin = origin;
}

void Ray::setDirection(glm::fvec3 dir)
{
	this->direction = glm::normalize(dir);
}

glm::fvec3 Ray::getOrigin()
{
	return origin;
}

glm::fvec3 Ray::getDirection()
{
	return direction;
}

bool Ray::intersectsPlane(Plane plane, glm::fvec3 * outHit)
{
	if (fabsf(glm::dot(plane.normal, direction)) < EPS) {
		if (fabsf(glm::dot(plane.normal, direction) - plane.d) < EPS) {
			if(outHit)
				*outHit = origin;
			return true;
		}
		return false;
	}
	float dist = (plane.d - glm::dot(plane.normal, origin))/glm::dot(plane.normal, direction);
	if (outHit)
		*outHit = origin + dist * direction;
	if (dist < 0) return false;
	return true;
}

bool Ray::intersectsPlane(glm::fvec3 normal, float d, glm::fvec3 * outHit)
{
	Plane plane;
	plane.normal = normal;
	plane.d = d;
	return intersectsPlane(plane, outHit);
}

bool Ray::intersectsTriangle(Triangle tri, glm::fvec3 * outHit, bool cullFaces)
{
	return intersectsTriangle(tri.v0, tri.v1, tri.v2, outHit, tri.ccw, cullFaces);
}

bool Ray::intersectsTriangle(glm::fvec3 A, glm::fvec3 B, glm::fvec3 C, glm::fvec3 * outHit, bool ccw, bool cullFaces)
{
	// Rearrange the triangle if definde clockwise:
	if (!ccw) return intersectsTriangle(A, C, B, outHit, true, cullFaces);

	glm::fvec3 AB = B - A;
	glm::fvec3 AC = C - A;
	glm::fvec3 h = glm::cross(direction, AC);
	float det = glm::dot(h, AB);

	// Ignore 'away-facing' triangles?
	if (cullFaces) {
		if (det < EPS) return false;
	}

	// Is the Ray parallel to the triangle?
	if (EPS > det && det > -EPS) return false;

	glm::fvec3 p = origin - A;
	float u = glm::dot(p, h) / det;
	if (u < 0.0f || u > 1.0f) return false;
	glm::fvec3 q = glm::cross(p, AB);
	float v = glm::dot(direction, q) / det;
	if (v < 0.0f || u + v > 1.0f) return false;

	float t = glm::dot(AC, q) / det;
	if (outHit) 
		*outHit = origin + t * direction;
	
	return true;
}

bool Ray::intersectsAABB(AABoundingBox box, glm::fvec3 * outHit)
{
	float width = box.width;
	float height = box.height;
	float depth = box.depth;

	glm::fvec3 center = box.position;
	// Does the Ray start inside the bounding box?
	if (origin.x <= center.x + width / 2  && origin.x >= center.x - width / 2
		&& origin.y <= center.y + height / 2 && origin.y >= center.y - height / 2
		   && origin.z <= center.z + depth / 2 && origin.z >= center.z - depth / 2) {
		if (outHit) *outHit = origin;
		return true;
	}
	// Figure out on which side of the bounding box the ray starts
	glm::fvec3 delta = origin - center;

	glm::fvec3 collision;
	
	// Check the planes facing towards the ray's origin:
	if (intersectsPlane(glm::fvec3(1.0f, 0.0f, 0.0f), center.x + sign(delta.x) * (width / 2.0f), &collision)) {
		if (collision.y <= center.y + height / 2 + EPS && collision.y >= center.y - height / 2 - EPS
			&& collision.z <= center.z + depth / 2 + EPS && collision.z >= center.z - depth / 2 - EPS) {
			if(outHit) *outHit = collision;
			return true;
		}
	}
	else if (intersectsPlane(glm::fvec3(0.0f, 1.0f, 0.0f), center.y + sign(delta.y) * (height / 2.0f), &collision)) {
		if (collision.x <= center.x + width / 2 + EPS && collision.x >= center.x - width / 2- EPS
			&& collision.z <= center.z + depth / 2 + EPS && collision.z >= center.z - depth / 2 - EPS) {
			if (outHit) *outHit = collision;
			return true;
		}
	}
	else if (intersectsPlane(glm::fvec3(0.0f, 0.0f, 1.0f), center.z + sign(delta.z) * (depth / 2.0f), &collision)) {
		if (collision.x <= center.x + width / 2 + EPS && collision.x >= center.x - width / 2 - EPS
			&& collision.y <= center.y + height / 2 + EPS && collision.y >= center.y - height / 2 - EPS) {
			if (outHit) *outHit = collision;
			return true;
		}
	}

	return false;
}

bool Ray::intersectsAABB(glm::fvec3 lowLftBck, glm::fvec3 uprRgtFwd, glm::fvec3 * outHit)
{
	AABoundingBox box;
	box.position = (lowLftBck + uprRgtFwd) / 2.0f;
	box.width = uprRgtFwd.x - lowLftBck.x;
	box.height = uprRgtFwd.y - lowLftBck.y;
	box.depth = uprRgtFwd.z - lowLftBck.z;
	return intersectsAABB(box, outHit);
}

bool Ray::intersectsAABB(glm::fvec3 center, float width, float height, float depth, glm::fvec3 * outHit)
{
	AABoundingBox box;
	box.position = center;
	box.width = width;
	box.height = height;
	box.depth = depth;
	return intersectsAABB(box, outHit);
}

bool Ray::intersectsBB(BoundingBox box, glm::fvec3 * outHit)
{
	return intersectsBB(box.transform, box.width, box.height, box.depth, outHit);
}

bool Ray::intersectsBB(Transform3D boxTransform, float width, float height, float depth, glm::fvec3 * outHit)
{
	// Rotate origin and direction inversely to the box transform, thus aligning the box with the axises
	glm::fvec3 originRotated = glm::fvec3(boxTransform.getTransformInverted() * glm::fvec4(origin, 1.0f));
	glm::fvec3 directionRotated = glm::fvec3(boxTransform.getTransformInverted() * glm::fvec4(direction, 0.0f));
	Ray ray = Ray(originRotated, directionRotated);

	// Now, use the algorithm for axis aligned bounding boxes:
	return ray.intersectsAABB(glm::fvec3(0.0f, 0.0f, 0.0f), width, height, depth, outHit);
}

bool Ray::intersectsBB(glm::fvec3 position, glm::fquat orientation, float width, float height, float depth, glm::fvec3 * outHit)
{
	Transform3D transform;
	transform.setPosition(position);
	transform.setOrientation(orientation);
	return intersectsBB(transform, width, height, depth, outHit);
}

bool Ray::intersectsSphere(Sphere sphere, glm::fvec3 * outHit)
{
	glm::fvec3 delta = origin - sphere.center;
	// Check whether the origin is inside the sphere:
	if (glm::length(delta) <= sphere.radius) {
		if (outHit) *outHit = origin;
		return true;
	}

	// The intersection of a Ray and a Sphere are given by the following equation: 
	// || point - center || = radius, where point = origin + t * direction.
	// This results in a quadratic equation when expanded and rearranged.
	float a = glm::dot(direction, direction);
	float b = 2 * glm::dot(direction, delta);
	float c = glm::dot(delta, delta) - sphere.radius * sphere.radius;

	float discriminant = b*b - 4 * a*c;

	// If the discriminant is negative, the equation has no solution, thus, there
	// is no intersection between the ray and the sphere.
	if (discriminant < 0)
		return false;
	
	float t = (-b - sqrt(discriminant)) / (2 * a);
	if (t < 0) t = (-b + sqrt(discriminant)) / (2 * a);

	// Is the point where the ray hits the sphere behind the origin?
	if (t < 0) return false;

	if (outHit) *outHit = origin + t * direction;
	return true;
}

bool Ray::intersectsSphere(Sphere sphere, glm::fvec3 * outHit, glm::fvec3 * outHitSecondary)
{
	glm::fvec3 delta = origin - sphere.center;
	// Check whether the origin is inside the sphere:
	if (glm::length(delta) <= sphere.radius) {
		if (outHit) *outHit = origin;
		return true;
	}

	// The intersection of a Ray and a Sphere are given by the following equation: 
	// || point - center || = radius, where point = origin + t * direction.
	// This results in a quadratic equation when expanded and rearranged.
	float a = glm::dot(direction, direction);
	float b = 2 * glm::dot(direction, delta);
	float c = glm::dot(delta, delta) - sphere.radius * sphere.radius;

	float discriminant = b*b - 4 * a*c;

	// If the discriminant is negative, the equation has no solution, thus, there
	// is no intersection between the ray and the sphere.
	if (discriminant < 0)
		return false;

	float t = (-b - sqrt(discriminant)) / (2 * a);
	if (t < 0) t = (-b + sqrt(discriminant)) / (2 * a);

	// Is the point where the ray hits the sphere behind the origin?
	if (t < 0) return false;

	if (outHit) *outHit = origin + t * direction;

	// Output second solution
	if (outHitSecondary) { 
		float t = (-b + sqrt(discriminant)) / (2 * a);
		*outHitSecondary = origin + t * direction;
	}
	return true;
}

bool Ray::intersectsSphere(glm::fvec3 center, float radius, glm::fvec3 * outHit)
{
	Sphere sphere;
	sphere.center = center;
	sphere.radius = radius;
	return intersectsSphere(sphere, outHit);
}

bool Ray::intersectsSphere(glm::fvec3 center, float radius, glm::fvec3 * outHit, glm::fvec3 * outHitSecondary)
{
	Sphere sphere;
	sphere.center = center;
	sphere.radius = radius;
	return intersectsSphere(sphere, outHit, outHitSecondary);
}

