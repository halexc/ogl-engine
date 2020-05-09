#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>
#include <map>

#include "Shader.h"
#include "Transform3D.h"

struct Vertex3D {
	glm::fvec3 pos;
	glm::fvec3 normal;
	glm::fvec2 tex;

	inline bool operator==(const Vertex3D & u) const {
		bool comparison = (u.pos == pos && u.normal == normal && u.tex == tex);
		return comparison;
	}
};


struct Triangle {
	int v0, v1, v2;
	int n0, n1, n2;
	int tex0, tex1, tex2;
};

class TriangleModel
{
public:
	TriangleModel();
	~TriangleModel();

	static TriangleModel* loadOBJ(const char * filePath);

	void draw(Shader s);
	void draw(GLenum mode, Shader s);
	void setTexture(GLuint tex);

	void invalidate();

	Transform3D * getTransform();
	void setTransformGlobal(glm::fmat4 model);

	void getPolygons(std::vector<Vertex3D> ** vertices, std::vector<unsigned int> ** indices);

private:
	GLuint texture = 0;
	GLuint VAO, VBO, EBO;

	bool valid = false;

	Transform3D transform = Transform3D();

	void setupBuffers();

	std::map<int, glm::fvec3> vertices;
	std::map<int, glm::fvec3> vertexNormals;
	std::map<int, glm::fvec2> texCoords;

	std::vector<Triangle> tris;

	std::vector<unsigned int> indices;
	std::vector<Vertex3D> faceVertices;

	bool isVertexKnown(Vertex3D v, int& index);

	static int hashVertex3D(Vertex3D v) {
		std::hash<int> intHash;
		size_t v1 = intHash(v.pos.x) ^ (intHash(v.pos.y) << 1) ^ (intHash(v.pos.z) << 2);
		size_t v2 = intHash(v.normal.x) ^ (intHash(v.normal.y) << 1) ^ (intHash(v.normal.z) << 2);
		size_t v3 = intHash(v.tex.x) ^ (intHash(v.tex.y) << 1);
		return (int) (v1 ^ (v2 << 1) ^ (v3 << 2));
	}
};

