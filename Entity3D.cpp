#include "Entity3D.h"



Entity3D::Entity3D()
{
}


Entity3D::~Entity3D()
{
	for (Component * c : components)
		delete c;
	components.clear();
}

void Entity3D::update(double delta)
{
	for (Component * c : components)
		c->update(delta);
}

void Entity3D::draw()
{
	for (Component * c : components)
		c->draw();
}

Transform3D * Entity3D::getTransform()
{
	return &transform;
}

void Entity3D::addComponent(Component * c)
{
	components.push_back(c);
	c->setParentTransform(&transform);
}

Component * Entity3D::getComponent(unsigned int i)
{
	if (i >= components.size()) {
		return NULL;
	}
	return components[i];
}

unsigned int Entity3D::getNumComponents()
{
	return components.size();
}

Component * Entity3D::removeComponent(unsigned int i)
{
	if (i >= components.size()) {
		return NULL;
	}
	Component * ret = components[i];
	components.erase(components.begin() + i);
	return ret;
}

Component * Entity3D::removeComponent(Component * c)
{
	if (c == NULL) return NULL;

	std::vector<Component *>::iterator it = std::find(components.begin(), components.end(), c);
	if (it != components.end()) {
		components.erase(it);
		return c;
	}
	return NULL;
}

void Entity3D::deleteComponents()
{
	for (Component * c : components)
		delete c;
	components.clear();
}
