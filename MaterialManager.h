#pragma once

#include <vector>
#include <map>
#include <string>

#include "Material.h"
#include "Shader.h"
#include "Camera.h"

#include "assimp\Importer.hpp"
#include "assimp\scene.h"
#include "assimp\postprocess.h"

class MaterialManager
{
public:
	MaterialManager();
	~MaterialManager();

	Material * loadFromAiMaterial(aiMaterial * mat);
	Material * loadFromAiMaterial(std::string name, aiMaterial * mat);
	Material * loadFromAiMaterial(aiMaterial * mat, bool ambientFromDiffuse);
	Material * loadFromAiMaterial(std::string name, aiMaterial * mat, bool ambientFromDiffuse);

	void updateShaders(Camera * c);
	void updateShaders(glm::fmat4 projection, glm::fmat4 view);

	void addShader(std::string nameShader, Shader * shader);
	void addShader(const std::string pathVS, const std::string pathFS);
	void addShader(const std::string name, const std::string pathVS, const std::string pathFS);

	void addMaterial(Material * mat);
	void addMaterial(aiMaterial * mat);
	void addMaterial(std::string nameMaterial, Material * mat);

	void loadTexture(const std::string path, GLuint * outTexture = NULL);

	Shader * getShader(std::string name);
	std::map<std::string, Shader *> * getShaders();
	
	Material * getMaterial(std::string name);
	GLuint getTexture(std::string path);
	GLuint getWhiteTexture();

private:
	void addTexturesFromMaterial(Material * res, aiMaterial * mat);

	GLuint texWhite;

	std::map<std::string, Shader *> shaders;
	std::map<std::string, Material *> materials;
	std::map<std::string, GLuint> textureAtlas;
};

