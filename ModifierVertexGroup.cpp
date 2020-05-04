#include "ModifierVertexGroup.h"
#include "glm/gtc/matrix_transform.hpp"

ModifierVertexGroup::ModifierVertexGroup()
{
}

ModifierVertexGroup::~ModifierVertexGroup()
{
}

void ModifierVertexGroup::loadModelData(TriangleModel * obj)
{
	loadModelData(obj, 0.0f);
}

void ModifierVertexGroup::loadModelData(TriangleModel * obj, float defaultWeight)
{
	ModelModifier::loadModelData(obj);
	if (indices == NULL) return;
	for (unsigned int i : *indices) {
		weights.insert(std::pair<unsigned int, float>(i, defaultWeight));
	}
}

void ModifierVertexGroup::transform(glm::fmat4 transformation)
{
	glm::fmat4 T = transformation * modifierMatrix;
	for (int i = 0; i < vertices->size(); i++) {
		if (weights.at(0) == 0.0f) continue;

		glm::fvec3 oldPos = vertices->at(i).pos;
		glm::fvec3 oldNormal = vertices->at(i).normal;

		glm::fvec4 v = glm::fvec4(oldPos, 1.0f);
		glm::fvec4 n = glm::fvec4(oldNormal, 1.0f);

		glm::fvec4 newPos = T * v;
		glm::fvec4 newNormal = T * n;

		vertices->at(i).pos = oldPos + weights.at(i) * (glm::fvec3(newPos.x, newPos.y, newPos.z) - oldPos);
		vertices->at(i).normal = oldNormal + weights.at(i) * (glm::fvec3(newNormal.x, newNormal.y, newNormal.z) - oldNormal);
	}
	obj->invalidate();
}

void ModifierVertexGroup::setWeight(unsigned int vertexID, float weight)
{
	std::map<unsigned int, float>::iterator it = weights.find(vertexID);
	if (it != weights.end()) {
		it->second = weight;
	}
	else {
		weights.insert(std::pair<unsigned int, float>(vertexID, weight));
	}
}

TriangleModel * ModifierVertexGroup::getParentModel()
{
	return obj;
}
