#pragma once

#include <glm\glm.hpp>
#include "Shader.h"

#define MAX_NUM_POINT_LIGHTS 16
#define MAX_NUM_DIR_LIGHTS 4

// Number of definition layers each shadow map should have (must be the same as in the shaders)
#define NUM_SHADOW_LAYERS 3

class Scene;

class Light
{
public:
	Light();
	Light(unsigned int width, unsigned int height);
	Light(bool shadows, unsigned int width = 1024, unsigned int height = 1024);
	~Light();

	// Set the Light's color by passing a glm::fvec3 containing the RGB components.
	void setColor(glm::fvec3 color);
	// Set the Light's color by passing the three RGB components seperately.
	void setColor(float r, float g, float b);
	// Get the Light's color. The return value is a glm::fvec3 containing the RGB components
	glm::fvec3 getColor();

	// Returns whether this Light casts shadows.
	bool castsShadows();

	// Set the intensity of the Light by passing a float.
	void setIntensity(float i);
	// Get the intensity of the Light. Return value is a float.
	float getIntensity();

	GLuint getShadowMap();

	// Abstract function prototype that is implemented based on the kind of Light.
	virtual void configureShader(Shader * s, unsigned int i) = 0;
	// Abstract function prototype that is implemented based on the kind of Light.
	virtual void drawShadows(Scene * scene, Shader * depthShader) = 0;

protected:
	bool castShadows;
	GLuint FBO = 0;
	GLuint texFBO = 0;
	
	bool generateShadowTexArray(unsigned int width, unsigned int height);
	void drawShadowMapDirectional(Scene * scene, Shader * depthShader, glm::fmat4 projection, int layer);

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

	// Set the position of the PointLight by passing a glm::fvec3.
	void setPosition(glm::fvec3 pos);
	// Set the position of the PointLight by passing x, y, and z coordinates.
	void setPosition(float x, float y, float z);
	// Get the position of the PointLight. The return type is glm::fvec3.
	glm::fvec3 getPosition();

	// Configure the shader uniforms to prepare them for drawing. The lights should be 
	// arranged in a struct array. For PointLights specifically, the array containing all the
	// PointLights should be named 'pLight'. The PointLight struct in the shader has
	// the following attributes:
	// .pos (the light's position)
	// .color (the light's color)
	// .intensity (the light's brightness)
	// The necessary parameters are a pointer to the Shader to configure as well as the
	// index of the light.
	virtual void configureShader(Shader * s, unsigned int i);

	// Render the shadow maps to the corresponding texture layers. This action should be performed
	// BEFORE drawing the actual scene, since otherwise the shadow maps might be outdated.
	// The necessary parameters are a pointer to the Scene object which casts the shadows as well
	// as a pointer to the depth shader which is used for rendering the shadows.
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

	// Set the direction of the DirectionalLight by passing a glm::fvec3.
	void setDirection(glm::fvec3 dir);
	// Set the direction of the DirectionalLight by passing the respective x, y, and z values of
	// the direction vector.
	void setDirection(float x, float y, float z);
	// Get the current direction of the DirectionalLight. The return value is the direction vector
	// of type glm::fvec3.
	glm::fvec3 getDirection();

	// Set the intensity of ambient light of the DirectionalLight. This only applies to the light
	// intensity on surfaces that do not face the light, but rather only the strength of the
	// ambient light.
	// The necessary parameter is of type float and should be greater or equal 0.
	void setAmbientIntensity(float f);
	// Returns the ambient light intensity of the DirectionalLight. The return type is float.
	float getAmbientIntensity();

	// Configure the shader uniforms to prepare them for drawing. The lights should be 
	// arranged in a struct array. For DirectionalLights specifically, the array containing all
	// the DirectionalLights should be named 'dLight'. The DirectionalLight struct in the 
	// shader has the following attributes:
	// .dir (the light's direction)
	// .color (the light's color)
	// .intensity (the light's brightness)
	// .ambientIntensity (the global illumination provided by the light)
	// .index (the light's index)
	// The necessary parameters are a pointer to the Shader to configure as well as the
	// index of the light.
	virtual void configureShader(Shader * s, unsigned int i);

	// Render the shadow maps to the corresponding texture layers. This action should be performed
	// BEFORE drawing the actual scene, since otherwise the shadow maps might be outdated.
	// The necessary parameters are a pointer to the Scene object which casts the shadows as well
	// as a pointer to the depth shader which is used for rendering the shadows.
	virtual void drawShadows(Scene * scene, Shader * depthShader);

protected:
	glm::fvec3 direction;
	float ambientIntesity = 0.2f;

	glm::fmat4 lightSpace[NUM_SHADOW_LAYERS];
};

// Directional Light, but with attenuation, starting at a certain position
class Flashlight : public DirectionalLight
{
public:
	Flashlight();
	Flashlight(unsigned int width, unsigned int height);
	Flashlight(bool shadows, unsigned int width = 1024, unsigned int height = 1024);

	virtual void configureShader(Shader * s, unsigned int i);
	virtual void drawShadows(Scene * scene, Shader * depthShader);
};

