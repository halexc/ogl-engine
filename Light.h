#pragma once

#include <glm\glm.hpp>
#include "Shader.h"

#define MAX_NUM_LIGHTS 16

class Light
{
public:

	void setPosition(glm::fvec3 pos);
	void setPosition(float x, float y, float z);
	glm::fvec3 getPosition();

	void setColor(glm::fvec3 color);
	void setColor(float r, float g, float b);
	glm::fvec3 getColor();

	void setIntensity(float i);
	float getIntensity();

	void configureShader(Shader * s, unsigned int i);

private:
	glm::fvec3 position;
	glm::fvec3 color;

	float intensity;
};

