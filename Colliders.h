#pragma once

#include "glm/glm.hpp"
#include "Transform3D.h"

struct Collider {
	virtual ~Collider() {};
	// Colliders may only intersect with other colliders if they are in the same layer.
	char layer = 0x01;

	// Still colliders do not interact with each other, only with non-still colliders.
	bool still = false;
};

struct Plane : Collider {
	glm::fvec3 normal;
	float d;
};

struct Triangle : Collider {
	glm::fvec3 v0;
	glm::fvec3 v1;
	glm::fvec3 v2;
	bool ccw = true;
};

// Axis-aligned bounding box
struct AABoundingBox : Collider {
	// Position of the bounding box's center
	glm::fvec3 position;
	float width;
	float height;
	float depth;
};

// Free bounding box
struct BoundingBox : Collider {
	// Instead of being defined by opposing corners, a regular bounding box
	// might also be rotated. Thus, the regular bounding box is defined by
	// a Transform3D and width, height, and depth floats.
	Transform3D transform;
	float width;
	float height;
	float depth;
};

// Sphere
struct Sphere : Collider {
	glm::fvec3 center;
	float radius;
};


#define EPS 0.000001f

class Ray
{
public:
	Ray();
	Ray(glm::fvec3 origin, glm::fvec3 dir);
	~Ray();

	void setOrigin(glm::fvec3 origin);
	void setDirection(glm::fvec3 dir);

	glm::fvec3 getOrigin();
	glm::fvec3 getDirection();

	bool intersectsPlane(Plane plane, glm::fvec3 * outHit = NULL);
	bool intersectsPlane(glm::fvec3 normal, float d, glm::fvec3 * outHit = NULL);

	bool intersectsTriangle(Triangle tri, glm::fvec3 * outHit = NULL, bool cullFaces = true);
	bool intersectsTriangle(glm::fvec3 A, glm::fvec3 B, glm::fvec3 C, glm::fvec3 * outHit = NULL, bool ccw = true, bool cullFaces = true);

	bool intersectsAABB(AABoundingBox box, glm::fvec3 * outHit = NULL);
	bool intersectsAABB(glm::fvec3 lowLftFwd, glm::fvec3 uprRgtBck, glm::fvec3 * outHit = NULL);
	bool intersectsAABB(glm::fvec3 center, float width, float height, float depth, glm::fvec3 * outHit = NULL);

	bool intersectsBB(BoundingBox box, glm::fvec3 * outHit = NULL);
	bool intersectsBB(Transform3D boxTransform, float width, float height, float depth, glm::fvec3 * outHit = NULL);
	bool intersectsBB(glm::fvec3 position, glm::fquat orientation , float width, float height, float depth, glm::fvec3 * outHit = NULL);

	bool intersectsSphere(Sphere sphere, glm::fvec3 * outHit = NULL);
	bool intersectsSphere(Sphere sphere, glm::fvec3 * outHit, glm::fvec3 * outHitSecondary);
	bool intersectsSphere(glm::fvec3 center, float radius, glm::fvec3 * outHit = NULL);
	bool intersectsSphere(glm::fvec3 center, float radius, glm::fvec3 * outHit, glm::fvec3 * outHitSecondary);

private:
	glm::fvec3 origin;
	glm::fvec3 direction;
};

