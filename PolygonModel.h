#pragma once

#include "assimp\Importer.hpp"
#include "assimp\scene.h"
#include "assimp\postprocess.h"

#include "Component.h"

#include "Material.h"
#include "MaterialManager.h"

#include "Shader.h"
#include "Transform3D.h"

#include <vector>
#include <string>

struct Vertex {
	glm::fvec3 position;
	glm::fvec3 normal;
	glm::fvec2 uv;
	glm::fvec4 color;
	glm::fvec3 tangent;
	glm::fvec3 bitangent;
};

class PolygonModel : public Component
{
public:
	PolygonModel();
	PolygonModel(aiMesh * mesh);
	PolygonModel(aiMesh * mesh, aiMaterial * mat);
	PolygonModel(aiMesh * mesh, Material * mat);
	PolygonModel(aiMesh * mesh, aiMaterial * mat, MaterialManager * matManager);
	~PolygonModel();

	virtual void update(double delta) {};
	virtual void draw();

	void setMaterial(Material * mat);
	Material * getMaterial();

	void setName(std::string name);
	std::string getName();

	void invalidate();
	void deleteResources();

private:
	GLuint VAO, VBO, EBO;
	bool valid = false;
	void setupBuffers();

	Material * mat;
	std::string name;

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	void loadVertices(aiVector3D* vertices, aiVector3D* normals, aiVector3D* uvs,
						aiColor4D* color, aiVector3D* tangents, aiVector3D* bitangents, unsigned int nVertices);
	void loadFaces(aiFace* faces, unsigned int nFaces);
};

