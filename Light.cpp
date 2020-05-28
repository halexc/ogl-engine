#include "Light.h"
#include "Scene.h"
#include "Utils.h"

Light::Light()
{
	castShadows = true;
	shadowWidth = 1024;
	shadowHeight = 1024;
	Utils::GenerateDepthFBO(&FBO, &texFBO, shadowWidth, shadowHeight);
}

Light::Light(unsigned int width, unsigned int height)
{
	castShadows = true;
	shadowWidth = width;
	shadowHeight = height;
	Utils::GenerateDepthFBO(&FBO, &texFBO, shadowWidth, shadowHeight);
}

Light::Light(bool shadows, unsigned int width, unsigned int height)
{
	castShadows = shadows;
	shadowWidth = width;
	shadowHeight = height;
	if (castShadows) {
		Utils::GenerateDepthFBO(&FBO, &texFBO, shadowWidth, shadowHeight);
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

GLuint Light::getShadowMap()
{
	return texFBO;
}

bool Light::generateShadowTexArray(unsigned int width, unsigned int height)
{
	GLint framebufferOld;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &framebufferOld);

	// Create Framebuffer Object.
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	// Create Texture.
	glGenTextures(1, &texFBO);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texFBO);
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_DEPTH_COMPONENT, width, height, 3 * MAX_NUM_DIR_LIGHTS);
	// Configure Texture.
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);

	glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texFBO, 0, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) return false;

	glBindFramebuffer(GL_FRAMEBUFFER, framebufferOld);
	return true;
}

void Light::drawShadowMapDirectional(Scene * scene, Shader * depthShader, glm::fmat4 projection)
{

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	GLint fboReadOld, fboDrawOld;
	glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &fboDrawOld);
	glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &fboReadOld);

	glCullFace(GL_FRONT);
	
	depthShader->use();
	depthShader->setMat4("projectionView", projection);

	glViewport(0, 0, shadowWidth, shadowHeight);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texFBO, 0);
	glClear(GL_DEPTH_BUFFER_BIT);

	for (int i = 0; i < scene->getNumEntities(); i++) {
		if (PolygonModel * model = scene->getEntity3D(i)->getComponent<PolygonModel>()) {
			if(model->castsShadows()) 
				model->draw(depthShader);
		}
	}

	// Reset state:
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboDrawOld);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fboReadOld);
	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

	glCullFace(GL_BACK);
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
	s->use();
	if (i >= MAX_NUM_POINT_LIGHTS) {
		std::cerr << "Light.cpp: Unable to configure shader. MAX_NUM_LIGHTS exceeded." << std::endl;
		return;
	}
	std::string pLight = "pLight[";
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

DirectionalLight::DirectionalLight() : Light(2048U, 2048U)
{
}

DirectionalLight::DirectionalLight(unsigned int width, unsigned int height) : Light(width, height)
{
}

DirectionalLight::DirectionalLight(bool shadows, unsigned int width, unsigned int height) : Light(shadows, width, height)
{
}

void DirectionalLight::setDirection(glm::fvec3 dir)
{
	direction = glm::normalize(dir);
}

void DirectionalLight::setDirection(float x, float y, float z)
{
	direction = glm::normalize(glm::fvec3(x, y, z));
}

glm::fvec3 DirectionalLight::getDirection()
{
	return direction;
}

void DirectionalLight::setAmbientIntensity(float f)
{
	ambientIntesity = f;
}

float DirectionalLight::getAmbientIntensity()
{
	return ambientIntesity;
}

void DirectionalLight::configureShader(Shader * s, unsigned int i)
{
	s->use();
	if (i >= MAX_NUM_DIR_LIGHTS) {
		std::cerr << "Light.cpp: Unable to configure shader. MAX_NUM_LIGHTS exceeded." << std::endl;
		return;
	}
	std::string pLight = "dLight[";
	pLight.append(std::to_string(i));
	pLight.append("]");
	std::string tmp(pLight);
	s->setVec3(tmp.append(".dir"), direction);
	tmp = pLight;
	s->setVec3(tmp.append(".color"), color);
	tmp = pLight;
	s->setFloat(tmp.append(".intensity"), intensity);
	tmp = pLight;
	s->setFloat(tmp.append(".ambientIntensity"), ambientIntesity);

	if (castShadows) {
		tmp = pLight;
		s->setMat4(tmp.append(".lightSpace"), lightSpace);

		tmp = pLight;
		s->setInt(tmp.append(".shadowMap"), 5 + i);
		glActiveTexture(GL_TEXTURE5 + i);
		glBindTexture(GL_TEXTURE_2D, texFBO);
	}
}

void DirectionalLight::drawShadows(Scene * scene, Shader * depthShader)
{
	Camera * c = scene->getCamera();
	glm::fvec3 lightPos =  - 6.0f * direction;
	lightSpace = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, 0.0625f, 9.0f) * glm::lookAt(lightPos, lightPos + direction, glm::fvec3(0.0f, 1.0f, 0.0f));

	drawShadowMapDirectional(scene, depthShader, lightSpace);
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
