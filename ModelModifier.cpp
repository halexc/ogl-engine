#include "ModelModifier.h"

ModelModifier::ModelModifier()
{
}

ModelModifier::~ModelModifier()
{
}

void ModelModifier::loadModelData(TriangleModel * obj)
{
	this->obj = obj;
	obj->getPolygons(&vertices, &indices);
}
