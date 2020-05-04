#include "CollisionManager.h"

#include <map>
#include <set>

float CollisionManager::sign(float f) {
	return -1.0f + 2.0f * (f >= 0);
}

bool CollisionManager::checkCollision(Sphere A, Sphere B, glm::fvec3 * outHit)
{
	// Are the objects in the same collision layer?
	if (!(A.layer & B.layer)) return false;

	// Sphere collision: Are the objects less than their combined radii apart?
	float d = glm::length(B.center - A.center);
	if (d <= A.radius + B.radius) {
		if (outHit) {
			glm::fvec3 dir = glm::normalize(B.center - A.center);
			float dIntFromA = A.radius - (A.radius + B.radius - d) / 2;
			*outHit = A.center + dIntFromA * dir;
		}
		return true;
	}
	return false;
}

bool CollisionManager::checkCollision(Sphere sphere, Plane plane, glm::fvec3 * outHit)
{
	// Are the objects in the same collision layer?
	if (!(sphere.layer & plane.layer)) return false;

	// Sphere-Plane collision: Is the distance to the center of the circle from the plane less than the radius?
	float d = glm::dot(sphere.center, plane.normal) - plane.d;
	if (d <= sphere.radius) {
		if (outHit) {
			*outHit = sphere.center - d * plane.normal;
		}
		return true;
	}
	return false;
}

bool CollisionManager::checkCollision(Sphere sphere, BoundingBox box, glm::fvec3 * outHit)
{
	// Are the objects in the same collision layer?
	if (!(sphere.layer & box.layer)) return false;

	// Is the distance too large anyway?
	if (glm::length(box.transform.getPosition() - sphere.center) > glm::length(glm::fvec3(box.width, box.height, box.depth)) + sphere.radius) return false;

	// Transform sphere invertly to bounding box, then use the axis aligned bounding box algorithm:
	sphere.center = glm::fvec3(box.transform.getTransformInverted() * glm::fvec4(sphere.center, 1.0f));
	AABoundingBox aabox;
	aabox.layer = box.layer;
	aabox.position = glm::fvec3(0.0f, 0.0f, 0.0f);
	aabox.width = box.width;
	aabox.height = box.height;
	aabox.depth = box.depth;

	bool res = checkCollision(sphere, aabox, outHit);
	if (outHit) {
		*outHit = glm::fvec3(box.transform.getTransform() * glm::fvec4(*outHit, 1.0f));
	}
	return res;
}

bool CollisionManager::checkCollision(Sphere sphere, AABoundingBox aabox, glm::fvec3 * outHit)
{
	// Are the objects in the same collision layer?
	if (!(sphere.layer & aabox.layer)) return false;

	// Calculate the distance from the sphere's center to the edge of the bounding box:
	float deltaX = aabox.position.x - sphere.center.x;
	float deltaY = aabox.position.y - sphere.center.y;
	float deltaZ = aabox.position.z - sphere.center.z;
	deltaX -= sign(deltaX) * aabox.width / 2;
	deltaY -= sign(deltaY) * aabox.height / 2;
	deltaZ -= sign(deltaZ) * aabox.depth / 2;
	glm::fvec3 delta = glm::fvec3(deltaX, deltaY, deltaZ);

	// Is that distance less than the sphere's radius?
	if (glm::length(delta) <= sphere.radius) {
		if (outHit) {
			*outHit = sphere.center + delta;
		}
		return true;
	}
	return false;
}

bool CollisionManager::checkCollision(Sphere sphere, Triangle tri, glm::fvec3 * outHit)
{
	// Are the objects in the same collision layer?
	if (!(sphere.layer & tri.layer)) return false;

	glm::fvec3 v0c = sphere.center - tri.v0;
	glm::fvec3 v1c = sphere.center - tri.v1;
	glm::fvec3 v2c = sphere.center - tri.v2;

	// Raycast along all triangle edges:
	int hits = 0;
	glm::fvec3 hit1, hit2;

	Ray ray = Ray(tri.v0, tri.v1 - tri.v0);
	float edgeLength = glm::length(tri.v1 - tri.v0);
	if (ray.intersectsSphere(sphere, &hit1, &hit2)) {
		if (glm::length(hit1 - ray.getOrigin()) <= edgeLength) {
			if (outHit) {
				if (hit1 == hit2) {
					hits++;
					*outHit = hit1;
				}
				else {
					hits += 2;
					*outHit = (hit1 + hit2) / float(hits);
				}
			}
		}
	}
	ray = Ray(tri.v1, tri.v2 - tri.v1);
	if (ray.intersectsSphere(sphere, &hit1, &hit2)) {
		if (glm::length(hit1 - ray.getOrigin()) <= edgeLength) {
			if (outHit) {
				if (hit1 == hit2) {
					*outHit = *outHit * float(hits) + hit1;
					hits++;
					*outHit = *outHit / float(hits);
				}
				else {
					*outHit = *outHit * float(hits) + hit1 + hit2;
					hits += 2;
					*outHit = *outHit / float(hits);
				}
			}
		}
	}
	ray = Ray(tri.v2, tri.v0 - tri.v2);
	if (ray.intersectsSphere(sphere, &hit1, &hit2)) {
		if (glm::length(hit1 - ray.getOrigin()) <= edgeLength) {
			if (outHit) {
				if (hit1 == hit2) {
					*outHit = *outHit * float(hits) + hit1;
					hits++;
					*outHit = *outHit / float(hits);
				}
				else {
					*outHit = *outHit * float(hits) + hit1 + hit2;
					hits += 2;
					*outHit = *outHit / float(hits);
				}
			}
		}
	}
	return (hits > 0);
}

bool CollisionManager::checkCollision(Plane plane, Sphere sphere, glm::fvec3 * outHit)
{
	return checkCollision(sphere, plane, outHit);
}

bool CollisionManager::checkCollision(Plane A, Plane B, glm::fvec3 * outHit)
{
	// Are the objects in the same collision layer?
	if (!(A.layer & B.layer)) return false;

	// Are the planes not parallel?
	if (A.normal != B.normal && A.normal != -B.normal) {
		if (outHit) {
			float d = B.d - A.d;
			glm::fvec3 dir = glm::cross(B.normal, A.normal);
			float absX = fabsf(dir.x), absY = fabsf(dir.y), absZ = fabsf(dir.z);

			// The reference point will be in the zero-plane of the highest direction entry 
			// (e.g. (x, 0, 0) for dir = (x,y,z) with x > y and x > z)
			if (absX >= absY && absX >= absZ) {
				*outHit = glm::fvec3(d / (B.normal.x - A.normal.x), 0, 0);
			}
			else if (absY >= absX && absY >= absZ) {
				*outHit = glm::fvec3(0, d / (B.normal.y - A.normal.y), 0);
			}
			else {
				*outHit = glm::fvec3(0, 0, d / (B.normal.z - A.normal.z));
			}
		}
		return true;
	}
	if ((A.normal == B.normal && A.d == B.d) || (A.normal == -B.normal && A.d == -B.d)) {
		if (outHit) *outHit = A.d * A.normal;
		return true;
	}
	return false;
}

bool CollisionManager::checkCollision(Plane plane, BoundingBox box, glm::fvec3 * outHit)
{
	// Are the objects in the same collision layer?
	if (!(plane.layer & box.layer)) return false;

	// Iterate through all corners to find the closest to the 
	glm::fvec3 corners[8];
	bool rightSide[8];

	glm::fvec3 boxPos = box.transform.getPositionGlobal();
	glm::fvec3 boxRight = box.transform.getRightGlobal(), boxUp = box.transform.getUpGlobal(), boxFwd = box.transform.getForwardGlobal();

	// Load all corners into the array. The positions are such that to get to any neighbour, a bit has to be flipped.
	for (int i = 0; i < 8; i++) {
		corners[i] = boxPos + (((i % 2) / 2.0f - 1) * box.width) * boxRight + (((i / 2) / 2.0f - 1) * box.height) * boxUp + (((i / 4) / 2.0f - 1) * box.depth) * boxFwd;
		float d = glm::dot(corners[i], plane.normal) - plane.d;

		// On which side of the plane is this vertex?
		if (d < 0)
			rightSide[i] = false;
		else
			rightSide[i] = true;
	}
	bool allRight = true;
	bool allLeft = true;
	for (int i = 0; i < 8; i++) {
		allRight &= rightSide[i];
		allLeft &= !rightSide[i];
	}
	// Are all the vertices on the same side of the plane? If yes, then there is no way that the box intersects the plane.
	if (allLeft || allRight) return false;

	// If we don't have to calculate the position of the hit, we're done here. 
	// Otherwise, calculate the center of the intersection footprint.
	if (outHit) {
		// Set to store edged that have already been visited.
		std::set<std::pair<int, int>> edgesVisited;

		int hits = 0;

		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 3; j++) {
				int tmp_i = i;
				int tmp_j = i ^ (1 << j);

				// Order the temporary vars. This is for insertion into the set as a pair.
				if (tmp_i > tmp_j) {
					tmp_i = tmp_j;
					tmp_j = i;
				}

				if (edgesVisited.count(std::pair<int, int>(tmp_i, tmp_j))) continue;

				if (rightSide[tmp_i] != rightSide[tmp_j]) {
					edgesVisited.insert(std::pair<int, int>(tmp_i, tmp_j));
					glm::fvec3 hit;
					Ray ray = Ray(corners[tmp_i], corners[tmp_j] - corners[tmp_i]);

					if (ray.intersectsPlane(plane, &hit)) {
						*outHit = *outHit * float(hits) + hit;
						hits++;
						*outHit = *outHit / float(hits);
					}
				}
			}
		}
	}

	return true;
}

bool CollisionManager::checkCollision(Plane plane, AABoundingBox aabox, glm::fvec3 * outHit)
{
	// Iterate through all corners to find the closest to the 
	glm::fvec3 corners[8];
	bool rightSide[8];

	// Load all corners into the array. The positions are such that to get to any neighbour, a bit has to be flipped.
	for (int i = 0; i < 8; i++) {
		corners[i] = aabox.position + (((i % 2) / 2.0f - 1) * aabox.width) * glm::fvec3(1.0f, 0.0f, 0.0f) 
			+ (((i / 2) / 2.0f - 1) * aabox.height) * glm::fvec3(0.0f, 1.0f, 0.0f) + (((i / 4) / 2.0f - 1) * aabox.depth) * glm::fvec3(0.0f, 0.0f, 1.0f);
		float d = glm::dot(corners[i], plane.normal) - plane.d;

		// On which side of the plane is this vertex?
		if (d < 0)
			rightSide[i] = false;
		else
			rightSide[i] = true;
	}
	bool allRight = true;
	bool allLeft = true;
	for (int i = 0; i < 8; i++) {
		allRight &= rightSide[i];
		allLeft &= !rightSide[i];
	}
	// Are all the vertices on the same side of the plane? If yes, then there is no way that the box intersects the plane.
	if (allLeft || allRight) return false;

	// If we don't have to calculate the position of the hit, we're done here. 
	// Otherwise, calculate the center of the intersection footprint.
	if (outHit) {
		// Set to store edged that have already been visited.
		std::set<std::pair<int, int>> edgesVisited;

		int hits = 0;

		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 3; j++) {
				int tmp_i = i;
				int tmp_j = i ^ (1 << j);

				// Order the temporary vars. This is for insertion into the set as a pair.
				if (tmp_i > tmp_j) {
					tmp_i = tmp_j;
					tmp_j = i;
				}

				if (edgesVisited.count(std::pair<int, int>(tmp_i, tmp_j))) continue;

				if (rightSide[tmp_i] != rightSide[tmp_j]) {
					edgesVisited.insert(std::pair<int, int>(tmp_i, tmp_j));
					glm::fvec3 hit;
					Ray ray = Ray(corners[tmp_i], corners[tmp_j] - corners[tmp_i]);

					if (ray.intersectsPlane(plane, &hit)) {
						*outHit = *outHit * float(hits) + hit;
						hits++;
						*outHit = *outHit / float(hits);
					}
				}
			}
		}
	}

	return true;
}

bool CollisionManager::checkCollision(Plane plane, Triangle tri, glm::fvec3 * outHit)
{
	// Distances to the plane:
	float d[3];

	d[0] = glm::dot(tri.v0, plane.normal) - plane.d;
	d[1] = glm::dot(tri.v1, plane.normal) - plane.d;
	d[2] = glm::dot(tri.v2, plane.normal) - plane.d;

	bool allLeft = d[0] > 0 && d[1] > 0 && d[2] > 0;
	bool allRight = d[0] < 0 && d[1] < 0 && d[2] < 0;

	if (allLeft || allRight) return false;

	if (outHit) {
		int inPlane = 0;
		for (int i = 0; i < 3; i++) {
			if (d[i] == 0) inPlane++;
		}
	}

	return true;
}

bool CollisionManager::checkCollision(BoundingBox box, Sphere sphere, glm::fvec3 * outHit)
{
	return checkCollision(sphere, box, outHit);
}

bool CollisionManager::checkCollision(BoundingBox box, Plane plane, glm::fvec3 * outHit)
{
	return checkCollision(plane, box, outHit);
}

bool CollisionManager::checkCollision(BoundingBox A, BoundingBox B, glm::fvec3 * outHit)
{
	return false;
}

bool CollisionManager::checkCollision(BoundingBox box, AABoundingBox aabox, glm::fvec3 * outHit)
{
	return false;
}

bool CollisionManager::checkCollision(BoundingBox box, Triangle tri, glm::fvec3 * outHit)
{
	return false;
}

bool CollisionManager::checkCollision(AABoundingBox aabox, Sphere sphere, glm::fvec3 * outHit)
{
	return checkCollision(sphere, aabox, outHit);
}

bool CollisionManager::checkCollision(AABoundingBox aabox, Plane plane, glm::fvec3 * outHit)
{
	return checkCollision(plane, aabox, outHit);
}

bool CollisionManager::checkCollision(AABoundingBox aabox, BoundingBox box, glm::fvec3 * outHit)
{
	return checkCollision(box, aabox, outHit);
}

bool CollisionManager::checkCollision(AABoundingBox A, AABoundingBox B, glm::fvec3 * outHit)
{
	return false;
}

bool CollisionManager::checkCollision(AABoundingBox aabox, Triangle tri, glm::fvec3 * outHit)
{
	return false;
}

bool CollisionManager::checkCollision(Triangle tri, Sphere sphere, glm::fvec3 * outHit)
{
	return checkCollision(sphere, tri, outHit);
}

bool CollisionManager::checkCollision(Triangle tri, Plane plane, glm::fvec3 * outHit)
{
	return checkCollision(plane, tri, outHit);
}

bool CollisionManager::checkCollision(Triangle tri, BoundingBox box, glm::fvec3 * outHit)
{
	return checkCollision(box, tri, outHit);
}

bool CollisionManager::checkCollision(Triangle tri, AABoundingBox aabox, glm::fvec3 * outHit)
{
	return checkCollision(aabox, tri, outHit);
}

bool CollisionManager::checkCollision(Triangle A, Triangle B, glm::fvec3 * outHit)
{
	return false;
}
