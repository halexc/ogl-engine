#pragma once

#include "TriangleModel.h"
#include "ModelModifier.h"
class ModifierVertexGroup :
	public ModelModifier
{
public:

	ModifierVertexGroup();
	~ModifierVertexGroup();

	virtual void loadModelData(TriangleModel * obj);
	virtual void loadModelData(TriangleModel * obj, float defaultWeight);

	void transform(glm::fmat4 transformation);
	void setWeight(unsigned int vertexID, float weight);
	TriangleModel * getParentModel();

protected:
	std::map<unsigned int, float> weights;
};

