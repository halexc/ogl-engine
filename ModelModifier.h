#pragma once

#include "TriangleModel.h"

class ModelModifier
{
public:
	ModelModifier();
	~ModelModifier();

	virtual void loadModelData(TriangleModel * obj);

protected:
	glm::fmat4 modifierMatrix = glm::fmat4(1.0f);

	TriangleModel * obj = NULL;
	std::vector<Vertex3D> * vertices = NULL;
	std::vector<unsigned int> * indices = NULL;
};

