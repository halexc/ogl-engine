#include "pch.h"
#include "CppUnitTest.h"

#include "..\ogl-engine\Transform3D.h"
#include "..\ogl-engine\include\glm\glm.hpp"
#include "..\ogl-engine\include\glm\gtc\matrix_transform.hpp"

#include <cmath>

#define PI 3.14159265f
#define PI_2 PI/2.0f
#define EPS 0.00001f

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

void assertVec3(glm::fvec3 expected, glm::fvec3 actual) {
	for (int i = 0; i < 3; i++)
		Assert::IsTrue(fabsf(actual[i] - expected[i]) < EPS);
}

void assertQuat(glm::fquat expected, glm::fquat actual) {
	for (int i = 0; i < 4; i++)
		Assert::IsTrue(fabsf(actual[i] - expected[i]) < EPS);
}

void assertMatrix3(glm::fmat3 expected, glm::fmat3 actual) {
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			Assert::IsTrue(fabsf(actual[i][j] - expected[i][j]) < EPS);
}

void assertMatrix4(glm::fmat4 expected, glm::fmat4 actual) {
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			Assert::IsTrue(fabsf(actual[i][j] - expected[i][j]) < EPS);
}

namespace UnitTestEntities
{
	TEST_CLASS(Transform3DTest)
	{
	public:
		
		TEST_METHOD(SimpleInitialization)
		{
			// Initialize a trivial Transform3D (unit matrix)
			Transform3D tf = Transform3D();
			glm::fmat4 expected = glm::fmat4(1.0f);
			glm::fmat4 actual = tf.getTransform();
			assertMatrix4(expected, actual);
			
			tf.translate(1.0f, 1.0f, 0.0f);
			expected = glm::translate(glm::fmat4(1.0f), glm::fvec3(1.0f, 1.0f, 0.0f));
			actual = tf.getTransform();
			assertVec3(glm::fvec3(1.0f, 1.0f, 0.0f), tf.getPosition());
			assertMatrix4(expected, actual);

			tf.rotate(PI_2, glm::fvec3(0.0f, 1.0f, 0.0f));
			expected *= glm::rotate(glm::fmat4(1.0f), PI_2, glm::fvec3(0.0f, 1.0f, 0.0f));
			actual = tf.getTransform();
			assertVec3(glm::fvec3(1.0f, 1.0f, 0.0f), tf.getPosition());
			assertMatrix4(expected, actual);

			tf.rotate(3 * PI_2, glm::fvec3(0.0f, 1.0f, 0.0f));
			expected = glm::translate(glm::fmat4(1.0f), glm::fvec3(1.0f, 1.0f, 0.0f));
			actual = tf.getTransform();
			assertVec3(glm::fvec3(1.0f, 1.0f, 0.0f), tf.getPosition());
			assertMatrix4(expected, actual);

			tf.translate(0.0f, -1.0f, 0.0f);
			assertVec3(glm::fvec3(1.0f, 0.0f, 0.0f), tf.getPosition());
			tf.rotateAround(PI_2, glm::fvec3(0.0f, 1.0f, 0.0f), glm::fvec3(-1.0f, 0.0f, 0.0f));
			expected = glm::rotate(glm::fmat4(1.0f), PI_2, glm::fvec3(0.0f, 1.0f, 0.0f)) * glm::translate(glm::fmat4(1.0f), glm::fvec3(1.0f, 0.0f, 0.0f));
			actual = tf.getTransform();
			assertVec3(glm::fvec3(0.0f, 0.0f, -1.0f), tf.getPosition());
			assertMatrix4(expected, actual);

			tf.scale(2.0f);
			expected = glm::scale(expected, glm::fvec3(2.0f, 2.0f, 2.0f));
			actual = tf.getTransform();
			assertVec3(glm::fvec3(0.0f, 0.0f, -1.0f), tf.getPosition());
			assertMatrix4(expected, actual);
		}

		TEST_METHOD(ParentTransformation)
		{
			Transform3D child = Transform3D();
			Transform3D parent = Transform3D();

			child.setParent(&parent);

			// Any transformation applied to the parent object is also indirectly applied to the child
			// transform. Thus, moving the parent up by 1 unit and the child right by 1 unit should result
			// in a total displacement of 1 unit right and 1 unit up for the child in the global scope.
			parent.translate(0.0f, 1.0f, 0.0f);
			child.translate(1.0f, 0.0f, 0.0f);
			glm::fmat4 expected = glm::translate(glm::fmat4(1.0f), glm::fvec3(1.0f, 1.0f, 0.0f));
			glm::fmat4 actual = child.getTransform();
			assertVec3(glm::fvec3(1.0f, 1.0f, 0.0f), child.getPositionGlobal());
			assertMatrix4(expected, actual);

			// Scaling the parent object should also scale up the translation of the child transform.
			// Thus, the new expected translation of the child should be 2 units along the x axis, which
			// is twice the original translation, since the parent object is scaled by factor 2.
			parent.scale(2.0f);
			expected = glm::translate(glm::fmat4(1.0f), glm::fvec3(2.0f, 0.0f, 0.0f)) * parent.getTransform();
			actual = child.getTransform();
			assertVec3(glm::fvec3(2.0f, 1.0f, 0.0f), child.getPositionGlobal());
			assertMatrix4(expected, actual);

			// Rotating the parent object around a point should equally rotate the child.
			parent.rotateAround(PI_2, glm::fvec3(0.0f, 0.0f, 1.0f), glm::fvec3(0.0f, -1.0f, 0.0f));
			expected = glm::rotate(glm::fmat4(1.0f), PI_2, glm::fvec3(0.0f, 0.0f, 1.0f)) * expected;
			actual = child.getTransform();
			assertVec3(glm::fvec3(-1.0f, 2.0f, 0.0f), child.getPositionGlobal());
			assertMatrix4(expected, actual);
		}
	};
}
