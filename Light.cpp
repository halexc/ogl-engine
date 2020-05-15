#include "Light.h"
#include "Scene.h"
#include "Utils.h"

Light::Light()
{
	castShadows = true;
	Utils::GenerateDepthFBO(&FBO, &texFBO, 1024, 1024);
}

Light::Light(unsigned int width, unsigned int height)
{
	castShadows = true;
	Utils::GenerateDepthFBO(&FBO, &texFBO, width, height);
}

Light::Light(bool shadows, unsigned int width, unsigned int height)
{
	castShadows = shadows;
	if (castShadows) {
		Utils::GenerateDepthFBO(&FBO, &texFBO, width, height);
	}
}

Light::~Light()
{
	if(FBO) glDeleteFramebuffers(1, &FBO);
	if(texFBO) glDeleteTextures(1, &texFBO);
}

void Light::setColor(glm::fvec3 color)
{
	this->color = color;
}

void Light::setColor(float r, float g, float b)
{
	color.x = r;
	color.y = g;
	color.z = b;
}

glm::fvec3 Light::getColor()
{
	return color;
}

bool Light::castsShadows()
{
	return castShadows;
}

void Light::setIntensity(float i)
{
	intensity = i;
}

float Light::getIntensity()
{
	return intensity;
}

void Light::drawShadows(Scene * scene, Shader * depthShader, glm::fmat4 projection)
{
	GLint fboReadOld, fboDrawOld;
	glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &fboDrawOld);
	glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &fboReadOld);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glClear(GL_DEPTH_BUFFER_BIT);

	depthShader->use();
	depthShader->setMat4("lightSpace", projection);

	for (int i = 0; i < scene->getNumEntities(); i++) {
		if (PolygonModel * model = scene->getEntity3D(i)->getComponent<PolygonModel>()) {
			model->draw(depthShader);
		}
	}

	// Reset state:
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboDrawOld);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fboReadOld);
}

PointLight::PointLight() : Light()
{
}

PointLight::PointLight(unsigned int width, unsigned int height) : Light(width, height)
{
}

PointLight::PointLight(bool shadows, unsigned int width, unsigned int height) : Light(shadows, width, height)
{
}

void PointLight::setPosition(glm::fvec3 pos)
{
	position = pos;
}

void PointLight::setPosition(float x, float y, float z)
{
	position.x = x;
	position.y = y;
	position.z = z;
}

glm::fvec3 PointLight::getPosition()
{
	return position;
}

void PointLight::configureShader(Shader * s, unsigned int i)
{
	if (i >= MAX_NUM_LIGHTS) {
		std::cerr << "Light.cpp: Unable to configure shader. MAX_NUM_LIGHTS exceeded." << std::endl;
		return;
	}
	std::string pLight = "light[";
	pLight.append(std::to_string(i));
	pLight.append("]");
	std::string tmp(pLight);
	s->setVec3(tmp.append(".pos"), position);
	tmp = pLight;
	s->setVec3(tmp.append(".color"), color);
	tmp = pLight;
	s->setFloat(tmp.append(".intensity"), intensity);
}

void PointLight::drawShadows(Scene * scene, Shader * depthShader)
{
}

DirectionalLight::DirectionalLight() : PointLight()
{
}

DirectionalLight::DirectionalLight(unsigned int width, unsigned int height) : PointLight(width, height)
{
}

DirectionalLight::DirectionalLight(bool shadows, unsigned int width, unsigned int height) : PointLight(shadows, width, height)
{
}

void DirectionalLight::setDirection(glm::fvec3 dir)
{
	direction = dir;
}

void DirectionalLight::setDirection(float x, float y, float z)
{
	direction = glm::fvec3(x, y, z);
}

glm::fvec3 DirectionalLight::getDirection()
{
	return direction;
}

void DirectionalLight::configureShader(Shader * s, unsigned int i)
{
}

void DirectionalLight::drawShadows(Scene * scene, Shader * depthShader)
{
}

Flashlight::Flashlight() : DirectionalLight()
{
}

Flashlight::Flashlight(unsigned int width, unsigned int height) : DirectionalLight(width, height)
{
}

Flashlight::Flashlight(bool shadows, unsigned int width, unsigned int height) : DirectionalLight(shadows, width, height)
{
}

void Flashlight::configureShader(Shader * s, unsigned int i)
{
}

void Flashlight::drawShadows(Scene * scene, Shader * depthShader)
{
}
