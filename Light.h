#pragma once

#include <glm\glm.hpp>
#include "Shader.h"

#define MAX_NUM_LIGHTS 16

class Light
{
public:
	void setColor(glm::fvec3 color);
	void setColor(float r, float g, float b);
	glm::fvec3 getColor();

	void setIntensity(float i);
	float getIntensity();

	virtual void configureShader(Shader * s, unsigned int i) = 0;

protected:
	glm::fvec3 color;
	float intensity;
};

class PointLight : public Light
{
public:
	void setPosition(glm::fvec3 pos);
	void setPosition(float x, float y, float z);
	glm::fvec3 getPosition();

	virtual void configureShader(Shader * s, unsigned int i);

protected:
	glm::fvec3 position;
};

class DirectionalLight : public Light
{
public:
	void setDirection(glm::fvec3 dir);
	void setDirection(float x, float y, float z);
	glm::fvec3 getDirection();

	virtual void configureShader(Shader * s, unsigned int i);

protected:
	glm::fvec3 direction;
};

class Flashlight : public PointLight, public DirectionalLight
{
public:
	virtual void configureShader(Shader * s, unsigned int i);
};

