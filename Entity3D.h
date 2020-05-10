#pragma once

#include "Component.h"
#include "Transform3D.h"

#include <vector>

class Entity3D
{
private:
	std::vector<Component*> components;
	Transform3D transform = Transform3D();

public:
	Entity3D();
	~Entity3D();

	void update(double delta);
	void draw();

	Transform3D * getTransform();

	void addComponent(Component * c);
	/*
	template <class T>
	T * getComponent<T>() {
		for (Component * c : components) {
			if (T * ret = dynamic_cast<T>(c)) 
				return ret;
		}
		return NULL;
	}
	*/
	Component * getComponent(unsigned int i);
	unsigned int getNumComponents();

	Component * removeComponent(unsigned int i);
	Component * removeComponent(Component * c);
	void deleteComponents();

};

