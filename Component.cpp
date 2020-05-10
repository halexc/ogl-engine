#include "Component.h"

void Component::setParentTransform(Transform3D * parent)
{
	parentTF = parent;
}

Transform3D * Component::getTransform()
{
	return parentTF;
}
