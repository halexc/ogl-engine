#pragma once

#include <glm\glm.hpp>
#include "Shader.h"

#define MAX_NUM_LIGHTS 16

class Scene;

class Light
{
public:
	Light();
	Light(unsigned int width, unsigned int height);
	Light(bool shadows, unsigned int width = 1024, unsigned int height = 1024);
	~Light();

	void setColor(glm::fvec3 color);
	void setColor(float r, float g, float b);
	glm::fvec3 getColor();

	bool castsShadows();

	void setIntensity(float i);
	float getIntensity();

	virtual void configureShader(Shader * s, unsigned int i) = 0;
	virtual void drawShadows(Scene * scene, Shader * depthShader) = 0;

protected:
	bool castShadows;
	GLuint FBO = 0;
	GLuint texFBO = 0;

	void drawShadows(Scene * scene, Shader * depthShader, glm::fmat4 projection);

	glm::fvec3 color;
	float intensity;
};

class PointLight : public Light
{
public:
	PointLight();
	PointLight(unsigned int width, unsigned int height);
	PointLight(bool shadows, unsigned int width = 1024, unsigned int height = 1024);

	void setPosition(glm::fvec3 pos);
	void setPosition(float x, float y, float z);
	glm::fvec3 getPosition();

	virtual void configureShader(Shader * s, unsigned int i);
	virtual void drawShadows(Scene * scene, Shader * depthShader);

protected:
	glm::fvec3 position;
};

// Light that is not emitted from a point but rather illuminates the scene from a certain direction
class DirectionalLight : public PointLight
{
public:
	DirectionalLight();
	DirectionalLight(unsigned int width, unsigned int height);
	DirectionalLight(bool shadows, unsigned int width = 1024, unsigned int height = 1024);

	void setDirection(glm::fvec3 dir);
	void setDirection(float x, float y, float z);
	glm::fvec3 getDirection();

	virtual void configureShader(Shader * s, unsigned int i);
	virtual void drawShadows(Scene * scene, Shader * depthShader);

protected:
	glm::fvec3 direction;
};

// Directional Light, but with attenuation
class Flashlight : public DirectionalLight
{
public:
	Flashlight();
	Flashlight(unsigned int width, unsigned int height);
	Flashlight(bool shadows, unsigned int width = 1024, unsigned int height = 1024);

	virtual void configureShader(Shader * s, unsigned int i);
	virtual void drawShadows(Scene * scene, Shader * depthShader);
};

