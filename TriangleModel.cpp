#include "TriangleModel.h"
#include <glm/gtc/type_ptr.hpp>

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <unordered_map>

TriangleModel::TriangleModel()
{
}

TriangleModel::~TriangleModel()
{
}

TriangleModel* TriangleModel::loadOBJ(const char * filePath)
{
	TriangleModel * res = new TriangleModel();
	std::ifstream infile(filePath);
	std::string line;

	int i_normals = 1;
	int i_vertices = 1;
	int i_texCoords = 1;

	std::unordered_map<int, int> mapVertices;

	while (infile.good() && std::getline(infile, line))
	{
		std::stringstream ssin(line);

		// Parse vertex data:
		if (line.at(0) == 'v') {
			// Texture (UV) coordinates:
			if (line.at(1) == 't') {
				char d;
				float x, y;
				ssin >> d >> d >> x >> y;
				glm::fvec2 texCoord(x, y);

				res->texCoords.insert(std::pair<int, glm::fvec2>(i_texCoords, texCoord));
				i_texCoords++;
			}
			// Vertex normals:
			else if (line.at(1) == 'n') {
				char d;
				float x, y, z;
				ssin >> d >> d >> x >> y >> z;
				glm::fvec3 normal(x, y, z);

				res->vertexNormals.insert(std::pair<int, glm::fvec3>(i_normals, normal));
				i_normals++;
			}
			// Vertices:
			else {
				char d;
				float x, y, z;
				ssin >> d >> x >> y >> z;
				glm::fvec3 vertex(x, y, z);

				res->vertices.insert(std::pair<int, glm::fvec3>(i_vertices, vertex));
				i_vertices++;
			}
		}
		// Parse faces:
		else if (line.at(0) == 'f') {
			char d;
			std::string set0, set1, set2;
			ssin >> d >> set0 >> set1 >> set2;

			Triangle tri;

			// OBJ face format: f [Vertex]/[TexCoord]/[Normal]
			std::stringstream ss0(set0);
			ss0 >> tri.v0 >> d >> tri.tex0 >> d >> tri.n0;
			std::stringstream ss1(set1);
			ss1 >> tri.v1 >> d >> tri.tex1 >> d >> tri.n1;
			std::stringstream ss2(set2);
			ss2 >> tri.v2 >> d >> tri.tex2 >> d >> tri.n2;

			res->tris.push_back(tri);

		}
	}
	unsigned int i = 0;
	for (Triangle tri : res->tris) {
		Vertex3D u, v, w;
		
		u.pos = res->vertices.at(tri.v0);
		u.normal = res->vertexNormals.at(tri.n0);
		u.tex = res->texCoords.at(tri.tex0);
		
		int index;
		if (res->isVertexKnown(u, index)) {
			res->indices.push_back(index);
		}
		else {
			res->indices.push_back(res->faceVertices.size());
			res->faceVertices.push_back(u);
		}

		v.pos = res->vertices.at(tri.v1);
		v.normal = res->vertexNormals.at(tri.n1);
		v.tex = res->texCoords.at(tri.tex1);

		if (res->isVertexKnown(v, index)) {
			res->indices.push_back(index);
		}
		else {
			res->indices.push_back(res->faceVertices.size());
			res->faceVertices.push_back(v);
		}

		w.pos = res->vertices.at(tri.v2);
		w.normal = res->vertexNormals.at(tri.n2);
		w.tex = res->texCoords.at(tri.tex2);

		if (res->isVertexKnown(w, index)) {
			res->indices.push_back(index);
		}
		else {
			res->indices.push_back(res->faceVertices.size());
			res->faceVertices.push_back(w);
		}

	}
	res->setupBuffers();
	return res;
}



void TriangleModel::draw(Shader s)
{
	if (!valid) {
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		//Array buffer for all vertices:
		glBufferData(GL_ARRAY_BUFFER, faceVertices.size() * sizeof(Vertex3D), &faceVertices[0], GL_STATIC_DRAW);

		valid = true;
	}

	s.setMat4("model", transform.getTransform());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);


}

void TriangleModel::draw(GLenum mode, Shader s)
{
	if (!valid) {
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		//Array buffer for all vertices:
		glBufferData(GL_ARRAY_BUFFER, faceVertices.size() * sizeof(Vertex3D), &faceVertices[0], GL_STATIC_DRAW);

		valid = true;
	}
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glBindVertexArray(VAO);
	for (int i = 0; i < indices.size(); i += 3)
		glDrawElements(mode, 3, GL_UNSIGNED_INT, (void*)(i * sizeof(GLuint)));
	glBindVertexArray(0);
}

void TriangleModel::setTexture(GLuint tex)
{
	this->texture = tex;
}

void TriangleModel::invalidate()
{
	valid = false;
}

Transform3D * TriangleModel::getTransform()
{
	return &transform;
}

void TriangleModel::setTransformGlobal(glm::fmat4 model)
{
	transform.setTransformGlobal(model);
}


void TriangleModel::getPolygons(std::vector<Vertex3D> ** vertices, std::vector<unsigned int> ** indices)
{
	*vertices = &(this->faceVertices);
	*indices = &(this->indices);
}

void TriangleModel::setupBuffers()
{
	//Setup buffers:
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//Array buffer for all vertices:
	glBufferData(GL_ARRAY_BUFFER, faceVertices.size() * sizeof(Vertex3D), &faceVertices[0], GL_STATIC_DRAW);

	//Element buffer for all triangles:
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, pos));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, normal));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, tex));

	glBindVertexArray(0);

	valid = true;
}

bool TriangleModel::isVertexKnown(Vertex3D v, int& index)
{
	for (int i = 0; i < faceVertices.size(); ++i) {
		Vertex3D u = faceVertices.at(i);
		if (u == v) {
			index = i;
			return true;
		}
	}
	return false;
}
