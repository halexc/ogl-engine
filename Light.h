#pragma once

#include <glm\glm.hpp>
#include "Shader.h"

#define MAX_NUM_POINT_LIGHTS 16
#define MAX_NUM_DIR_LIGHTS 4

#define DEBUG_LIGHTS 1

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

	GLuint getShadowMap();

	virtual void configureShader(Shader * s, unsigned int i) = 0;
	virtual void drawShadows(Scene * scene, Shader * depthShader) = 0;

protected:
	bool castShadows;
	GLuint FBO = 0;
	GLuint texFBO = 0;
	
	bool generateShadowTexArray(unsigned int width, unsigned int height);
	void drawShadowMapDirectional(Scene * scene, Shader * depthShader, glm::fmat4 projection);

	glm::fvec3 color;
	float intensity;
	unsigned int shadowWidth, shadowHeight;

};

// Light that is emitted equally in all directions, originating in a single point
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
class DirectionalLight : public Light
{
public:
	DirectionalLight();
	DirectionalLight(unsigned int width, unsigned int height);
	DirectionalLight(bool shadows, unsigned int width = 1024, unsigned int height = 1024);

	void setDirection(glm::fvec3 dir);
	void setDirection(float x, float y, float z);
	glm::fvec3 getDirection();

	void setAmbientIntensity(float f);
	float getAmbientIntensity();

	virtual void configureShader(Shader * s, unsigned int i);
	virtual void drawShadows(Scene * scene, Shader * depthShader);

protected:
	glm::fvec3 direction;
	float ambientIntesity = 0.2f;

	glm::fmat4 lightSpace = glm::fmat4(1.0f);
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

