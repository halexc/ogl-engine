#pragma once

#include "Transform3D.h"

class Component {
public:
	virtual void update(double delta) = 0;

	virtual void draw() { };

	void setParentTransform(Transform3D * parent);

	Transform3D * getTransform();

private:
	Transform3D * parentTF = NULL;
};