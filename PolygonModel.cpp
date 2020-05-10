#include "PolygonModel.h"

PolygonModel::PolygonModel()
{
	mat = new Material();
}

PolygonModel::PolygonModel(aiMesh * mesh)
{
	name = mesh->mName.C_Str();
	loadVertices(mesh->mVertices, mesh->mNormals, mesh->mTextureCoords[0], mesh->mColors[0], mesh->mTangents, mesh->mBitangents, mesh->mNumVertices);
	loadFaces(mesh->mFaces, mesh->mNumFaces);

	setupBuffers();
}

PolygonModel::PolygonModel(aiMesh * mesh, aiMaterial * mat) : PolygonModel(mesh)
{
	this->mat = new Material(mat);
}

PolygonModel::PolygonModel(aiMesh * mesh, Material * mat) : PolygonModel(mesh)
{
	this->mat = mat;
}

PolygonModel::PolygonModel(aiMesh * mesh, aiMaterial * mat, MaterialManager * matManager) : PolygonModel(mesh)
{
	this->mat = matManager->getMaterial(mat->GetName().C_Str());
}

PolygonModel::~PolygonModel()
{
	vertices.clear();
	indices.clear();
}

void PolygonModel::draw()
{
	if (mat->getShader() == NULL) {
		std::cerr << "PolygonModel.cpp: ERROR while drawing. No Shader available. Drawing process aborted..." << std::endl;
		return;
	}
	mat->prepare();
	if(getTransform()) mat->getShader()->setMat4("model", getTransform()->getTransform());

	if (!valid) {
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		//Array buffer for all vertices:
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		valid = true;
	}

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void PolygonModel::setMaterial(Material * mat)
{
	this->mat = mat;
}

Material * PolygonModel::getMaterial()
{
	return mat;
}

void PolygonModel::setName(std::string name)
{
	this->name = name;
}

std::string PolygonModel::getName()
{
	return name;
}

void PolygonModel::invalidate()
{
	valid = false;
}

void PolygonModel::deleteResources()
{
	mat->deleteResources();

	delete mat;
}

void PolygonModel::setupBuffers()
{
	//Setup buffers:
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//Array buffer for all vertices:
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	//Element buffer for all triangles:
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));

	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));

	glBindVertexArray(0);

	valid = true;
}

void PolygonModel::loadVertices(aiVector3D * vertices, aiVector3D * normals, aiVector3D * uvs, aiColor4D * colors, aiVector3D * tangents, aiVector3D * bitangents, unsigned int nVertices)
{
	for (unsigned int i = 0; i < nVertices; i++) {
		Vertex v = Vertex();
		v.position = glm::fvec3(vertices[i].x, vertices[i].y, vertices[i].z);
		if (normals) v.normal = glm::fvec3(normals[i].x, normals[i].y, normals[i].z);
		if (uvs) v.uv = glm::fvec2(uvs[i].x, uvs[i].y);
		if (colors) v.color = glm::fvec4(colors[i].r, colors[i].g, colors[i].b, colors[i].a);
		if (tangents) v.tangent = glm::fvec3(tangents[i].x, tangents[i].y, tangents[i].z);
		if (bitangents) v.bitangent = glm::fvec3(bitangents[i].x, bitangents[i].y, bitangents[i].z);

		this->vertices.push_back(v);
	}
}

void PolygonModel::loadFaces(aiFace * faces, unsigned int nFaces)
{
	for (unsigned int i = 0; i < nFaces; i++) {
		if (faces[i].mNumIndices != 3) {
			std::cerr << "PolygonModel.cpp: ERROR while loading faces. Some faces are not triangles" << std::endl;
		}
		for (unsigned int j = 0; j < 3; j++) {
			indices.push_back(faces[i].mIndices[j]);
		}
	}
}
