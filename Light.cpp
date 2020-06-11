#include "Light.h"
#include "Scene.h"
#include "Utils.h"

Light::Light()
{
	castShadows = true;
	shadowWidth = 1024;
	shadowHeight = 1024;
	generateShadowTexArray(shadowWidth, shadowHeight);
}

Light::Light(unsigned int width, unsigned int height)
{
	castShadows = true;
	shadowWidth = width;
	shadowHeight = height;
	generateShadowTexArray(shadowWidth, shadowHeight);
}

Light::Light(bool shadows, unsigned int width, unsigned int height)
{
	castShadows = shadows;
	shadowWidth = width;
	shadowHeight = height;
	if (castShadows) {
		generateShadowTexArray(shadowWidth, shadowHeight);
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
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_DEPTH_COMPONENT24, width, height, NUM_SHADOW_LAYERS);
	GLint success;
	glGetTexParameteriv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_IMMUTABLE_FORMAT, &success);
	// Configure Texture.
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	//glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texFBO, 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) return false;

	glBindFramebuffer(GL_FRAMEBUFFER, framebufferOld);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	return true;
}

void Light::drawShadowMapDirectional(Scene * scene, Shader * depthShader, glm::fmat4 projection, int layer)
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
	glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texFBO, 0, layer);
	glClear(GL_DEPTH_BUFFER_BIT);

	//depthShader->setInt("layer", layer);

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
	for (int i = 0; i < NUM_SHADOW_LAYERS; ++i) {
		lightSpace[i] = glm::fmat4(1.0f);
	}
}

DirectionalLight::DirectionalLight(unsigned int width, unsigned int height) : Light(width, height)
{
	for (int i = 0; i < NUM_SHADOW_LAYERS; ++i) {
		lightSpace[i] = glm::fmat4(1.0f);
	}
}

DirectionalLight::DirectionalLight(bool shadows, unsigned int width, unsigned int height) : Light(shadows, width, height)
{
	for (int i = 0; i < NUM_SHADOW_LAYERS; ++i) {
		lightSpace[i] = glm::fmat4(1.0f);
	}
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
		for (int j = 0; j < NUM_SHADOW_LAYERS; j++) {
			tmp = pLight;
			tmp.append(".lightSpace");
			if (NUM_SHADOW_LAYERS > 1) tmp.append("[").append(std::to_string(j)).append("]");
			s->setMat4(tmp, lightSpace[j]);
		}

		tmp = pLight;
		tmp = tmp.append(".shadowMap");
		s->setInt(tmp, 5 + i);
		glActiveTexture(GL_TEXTURE5 + i);
		glBindTexture(GL_TEXTURE_2D_ARRAY, texFBO);
	}
}

void DirectionalLight::drawShadows(Scene * scene, Shader * depthShader)
{
	Camera * c = scene->getCamera();
	
	for (int i = 0; i < NUM_SHADOW_LAYERS; i++) {
		glm::fvec3 lightPos = -3.0f * powf(10.0f, i) * direction;
		lightSpace[i] = glm::ortho(-2.0f * powf(10.0f, i), 2.0f * powf(10.0f, i), -2.0f * powf(10.0f, i), 2.0f * powf(10.0f, i), 0.0625f, 6.0f * powf(10.0f, i)) * glm::lookAt(lightPos, lightPos + direction, glm::fvec3(0.0f, 1.0f, 0.0f));
	}
	for (int i = 0; i < NUM_SHADOW_LAYERS; i++)
		drawShadowMapDirectional(scene, depthShader, lightSpace[i], i);
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
