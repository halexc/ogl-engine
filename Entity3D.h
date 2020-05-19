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

	static Entity3D * createQuad();

	void update(double delta);
	void draw();

	Transform3D * getTransform();

	void addComponent(Component * c);
	Component * getComponent(unsigned int i);
	unsigned int getNumComponents();

	Component * removeComponent(unsigned int i);
	Component * removeComponent(Component * c);
	void deleteComponents();


	template <typename T>
	T * getComponent() {
		for (Component * c : components) {
			if (T * ret = dynamic_cast<T*>(c))
				return ret;
		}
		return NULL;
	}

	template <typename T>
	int getNumComponentsOfType() {
		int i = 0;
		for (Component * c : components) {
			if (dynamic_cast<T>(c))
				i++;
		}
		return i;
	}

	template <typename T>
	T * getComponents(int * outNComponentsOfT = NULL) {
		int c = getNumComponentsOfType<T>();
		if (outNComponentsOfT) *outNComponentsOfT = c;
		T ret[c];
		int i = 0;
		for (Component * c : components) {
			if (T * t = dynamic_cast<T>(c)) {
				ret[i] = t;
				i++;
			}
		}
		return ret;
	}

};

