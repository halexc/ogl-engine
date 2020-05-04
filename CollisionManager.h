#pragma once

#include "Colliders.h"

class CollisionManager 
{
public:
	// Check whether there is a collision between two spheres. If a fvec3 pointer is passed, the vector's
	// value equals the center of the intersecting volume.
	static bool checkCollision(Sphere A,			Sphere B,				glm::fvec3 * outHit = NULL);

	// Check whether there is a collision between a sphere and a plane. If a fvec3 pointer is passed, the
	// vector's value equals the center of the footprint of the sphere on the plane.
	static bool checkCollision(Sphere sphere,		Plane plane,			glm::fvec3 * outHit = NULL);

	// Check whether there is a collision between a sphere and a bounding box. If a fvec3 pointer is passed,
	// the vector's value equals the center of the footprint of the sphere on the bounding box.
	static bool checkCollision(Sphere sphere,		BoundingBox box,		glm::fvec3 * outHit = NULL);

	// Check whether there is a collision between a sphere and an axis aligned bounding box. If a fvec3 pointer 
	// is passed, the vector's value equals the center of the footprint of the sphere on the bounding box.
	static bool checkCollision(Sphere sphere,		AABoundingBox aabox,	glm::fvec3 * outHit = NULL);

	// Check whether there is a collision between a sphere and a triangle. If a fvec3 pointer is passed, the
	// vector's value equals the center of the footprint of the sphere on the triangle.
	static bool checkCollision(Sphere sphere,		Triangle tri,			glm::fvec3 * outHit = NULL);
//---------------------------------------------------------------------------------------------------------------

	// Check whether there is a collision between a plane and a sphere. If a fvec3 pointer is passed, the
	// vector's value equals the center of the footprint of the sphere on the plane.
	static bool checkCollision(Plane plane,			Sphere sphere,			glm::fvec3 * outHit = NULL);

	// Check whether there is a collision between two planes. If a fvec3 pointer is passed, the vector's
	// value equals a point on the interecting line. The direction vector of the line can be retrieved
	// by taking the cross product of the two planes' normal vectors.
	static bool checkCollision(Plane A,				Plane B,				glm::fvec3 * outHit = NULL);

	// Check whether there is a collision between a plane and a bounding box. If a fvec3 pointer is passed,
	// the vector's value equals the center of the intersection footprint of the box on the plane.
	static bool checkCollision(Plane plane,			BoundingBox box,		glm::fvec3 * outHit = NULL);

	// Check whether there is a collision between a plane and an axis aligned bounding box. If a fvec3 
	// pointer is passed, the vector's value equals the center of the intersection footprint of the box 
	// on the plane.
	static bool checkCollision(Plane plane,			AABoundingBox aabox,	glm::fvec3 * outHit = NULL);

	// Check whether there is a collision between a plane and a triangle. If a fvec3 pointer is passed,
	// the vector's value equals the center of the intersection footprint of the triangle on the plane.
	static bool checkCollision(Plane plane,			Triangle tri,			glm::fvec3 * outHit = NULL);
//---------------------------------------------------------------------------------------------------------------

	// Check whether there is a collision between a bounding box and a sphere. If a fvec3 pointer is passed,
	// the vector's value equals the center of the footprint of the sphere on the bounding box.
	static bool checkCollision(BoundingBox box,		Sphere sphere,			glm::fvec3 * outHit = NULL);

	// Check whether there is a collision between a bounding box and a plane. If a fvec3 pointer is passed,
	// the vector's value equals the center of the intersection footprint of the box on the plane.
	static bool checkCollision(BoundingBox box,		Plane plane,			glm::fvec3 * outHit = NULL);

	// Check whether there is a collision between two bounding boxes. If a fvec3 pointer is passed, the 
	// vector's value equals the center of the intersecting volume.
	static bool checkCollision(BoundingBox A,		BoundingBox B,			glm::fvec3 * outHit = NULL);

	// Check whether there is a collision between a bounding box and an axis aligned bounding box. If a 
	// fvec3 pointer is passed, the vector's value equals the center of the intersecting volume.
	static bool checkCollision(BoundingBox box,		AABoundingBox aabox,	glm::fvec3 * outHit = NULL);

	// Check whether there is a collision between a bounding box and a triangle. If a fvec3 pointer 
	// is passed, the vector's value equals the center of the intersection footprint.
	static bool checkCollision(BoundingBox box,		Triangle tri,			glm::fvec3 * outHit = NULL);
//---------------------------------------------------------------------------------------------------------------

	// Check whether there is a collision between an axis aligned bounding box and a sphere. If a fvec3 pointer 
	// is passed, the vector's value equals the center of the footprint of the sphere on the bounding box.
	static bool checkCollision(AABoundingBox aabox, Sphere sphere,			glm::fvec3 * outHit = NULL);

	// Check whether there is a collision between an axis aligned bounding box and a plane. If a fvec3 
	// pointer is passed, the vector's value equals the center of the intersection footprint of the box 
	// on the plane.
	static bool checkCollision(AABoundingBox aabox, Plane plane,			glm::fvec3 * outHit = NULL);

	// Check whether there is a collision between a axis aligned bounding box and a bounding box. If a 
	// fvec3 pointer is passed, the vector's value equals the center of the intersecting volume.
	static bool checkCollision(AABoundingBox aabox, BoundingBox box,		glm::fvec3 * outHit = NULL);

	// Check whether there is a collision between two axis aligned bounding boxes. If a fvec3 pointer is
	// passed, the vector's value equals the center of the intersecting volume.
	static bool checkCollision(AABoundingBox A,		AABoundingBox B,		glm::fvec3 * outHit = NULL);

	// Check whether there is a collision between a axis aligned bounding box and a triangle. If a 
	// fvec3 pointer is passed, the vector's value equals the center of the intersection footprint.
	static bool checkCollision(AABoundingBox aabox, Triangle tri,			glm::fvec3 * outHit = NULL);
//---------------------------------------------------------------------------------------------------------------

	// Check whether there is a collision between a triangle and a sphere. If a fvec3 pointer is passed, the
	// vector's value equals the center of the footprint of the sphere on the triangle.
	static bool checkCollision(Triangle tri,		Sphere sphere,			glm::fvec3 * outHit = NULL);

	// Check whether there is a collision between a triangle and a plane. If a fvec3 pointer is passed,
	// the vector's value equals the center of the intersection footprint of the triangle on the plane.
	static bool checkCollision(Triangle tri,		Plane plane,			glm::fvec3 * outHit = NULL);

	// Check whether there is a collision between a triangle and a bounding box. If a fvec3 pointer 
	// is passed, the vector's value equals the center of the intersection footprint.
	static bool checkCollision(Triangle tri,		BoundingBox box,		glm::fvec3 * outHit = NULL);

	// Check whether there is a collision between a triangle and an axis aligned bounding box. If a 
	// fvec3 pointer is passed, the vector's value equals the center of the intersection footprint.
	static bool checkCollision(Triangle tri,		AABoundingBox aabox,	glm::fvec3 * outHit = NULL);

	// Check whether there is a collision between two triangles. If a fvec3 pointer is passed, the 
	// vector's value equals the center of the intersection footprint.
	static bool checkCollision(Triangle A,			Triangle B,				glm::fvec3 * outHit = NULL);
private:
	static float sign(float f);
};
