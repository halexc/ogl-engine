#include "MaterialManager.h"
#include "Utils.h"

MaterialManager::MaterialManager()
{
	//Create a default shader to use whenever no shader is available
	addShader("default", new Shader("Shader\\basicShader.vs", "Shader\\basicShader.fs"));
	addShader("depthShader", new Shader("Shader\\depthShader.vs", "Shader\\depthShader.fs"));

	//Create a default material to use when no other material is specified
	Material * defaultMat = new Material();
	defaultMat->setColorAmbient(0.75f, 0.75f, 0.75f);
	defaultMat->setColorDiffuse(0.75f, 0.75f, 0.75f);
	defaultMat->setShader(getShader("default"));

	materials.insert(std::pair<std::string, Material *>("default", defaultMat));
	textureAtlas.insert(std::pair<std::string, GLuint>("white", texWhite));
}

MaterialManager::~MaterialManager()
{
	materials.clear();
	shaders.clear();

	glDeleteTextures(1, &texWhite);

	for (std::map<std::string, GLuint>::iterator it = textureAtlas.begin(); it != textureAtlas.end(); it++) {
		GLuint tex = it->second;
		glDeleteTextures(1, &tex);
	}
	textureAtlas.clear();
}

Material * MaterialManager::loadFromAiMaterial(aiMaterial * mat)
{
	Material * res = new Material(mat);

	addTexturesFromMaterial(res, mat);

	res->setShader(shaders.at("default"));

	materials.insert(std::pair<std::string, Material *>(res->getName(), res));
	return res;
}

Material * MaterialManager::loadFromAiMaterial(std::string name, aiMaterial * mat)
{
	Material * res = new Material(name, mat);

	addTexturesFromMaterial(res, mat);

	res->setShader(shaders.at("default"));

	materials.insert(std::pair<std::string, Material *>(res->getName(), res));
	return res;
}

Material * MaterialManager::loadFromAiMaterial(aiMaterial * mat, bool ambientFromDiffuse)
{
	Material * res = new Material(mat, ambientFromDiffuse);

	addTexturesFromMaterial(res, mat);

	res->setShader(shaders.at("default"));

	materials.insert(std::pair<std::string, Material *>(res->getName(), res));
	return res;
}

Material * MaterialManager::loadFromAiMaterial(std::string name, aiMaterial * mat, bool ambientFromDiffuse)
{
	Material * res = new Material(name, mat, ambientFromDiffuse);

	addTexturesFromMaterial(res, mat);

	res->setShader(shaders.at("default"));

	materials.insert(std::pair<std::string, Material *>(res->getName(), res));
	return res;
}

void MaterialManager::updateShaders(Camera * c)
{
	for(std::map<std::string, Shader *>::iterator it = shaders.begin(); it != shaders.end(); it++)
		c->setShaderMatrices(it->second);
}

void MaterialManager::updateShaders(glm::fmat4 projection, glm::fmat4 view)
{
	for (std::map<std::string, Shader *>::iterator it = shaders.begin(); it != shaders.end(); it++) {
		Shader * s = (*it).second;
		s->setMat4("projection", projection);
		s->setMat4("view", view);
	}
}

void MaterialManager::addShader(std::string nameShader, Shader * shader)
{
	shaders.insert(std::pair<std::string, Shader *>(nameShader, shader));
}

void MaterialManager::addShader(const std::string pathVS, const std::string pathFS)
{
	Shader * shader = new Shader(pathVS.c_str(), pathFS.c_str());
	
	shaders.insert(std::pair<std::string, Shader *>(std::string(pathVS).append("+").append(pathFS), shader));
}

void MaterialManager::addShader(const std::string name, const std::string pathVS, const std::string pathFS)
{
	Shader * shader = new Shader(pathVS.c_str(), pathFS.c_str());

	shaders.insert(std::pair<std::string, Shader *>(name, shader));
}

void MaterialManager::addMaterial(Material * mat)
{
	materials.insert(std::pair<std::string, Material *>(mat->getName(), mat));
}

void MaterialManager::addMaterial(aiMaterial * mat)
{
	loadFromAiMaterial(mat);
}

void MaterialManager::addMaterial(std::string nameMaterial, Material * mat)
{
	materials.insert(std::pair<std::string, Material *>(nameMaterial, mat));
}

void MaterialManager::loadTexture(const std::string path, GLuint * outTexture)
{
	Utils::LoadTextureFromImage(outTexture, path.c_str());
	textureAtlas.insert(std::pair<std::string, GLuint>(path, *outTexture));
}

Shader * MaterialManager::getShader(std::string name)
{
	if(shaders.count(name))
		return shaders.at(name);
	std::cerr << "MaterialManager.cpp: ERROR when retrieving shader. No such shader was found. Returning default shader instead..." << std::endl;
	return shaders.at("default");
}

std::map<std::string, Shader*>* MaterialManager::getShaders()
{
	return &shaders;
}

Material * MaterialManager::getMaterial(std::string name)
{
	if(materials.count(name))
		return materials.at(name);
	std::cerr << "MaterialManager.cpp: ERROR when retrieving material. No such material was found. Returning default material instead..." << std::endl;
	return materials.at("default");
}

GLuint MaterialManager::getTexture(std::string path)
{
	// Is the texture already loaded?
	if (textureAtlas.count(path)) {
		return textureAtlas.at(path);
	}

	// If not, load the texture.
	GLuint tex;
	loadTexture(path, &tex);
	return tex;
}

GLuint MaterialManager::getWhiteTexture()
{
	return texWhite;
}

void MaterialManager::addTexturesFromMaterial(Material * res, aiMaterial * mat)
{
	if (res == NULL || mat == NULL) {
		std::cerr << "OOPS! No material passed when trying to laod textures..." << std::endl;
		return;
	}
	if (GLuint tex = res->getTexAmbient()) {
		aiString str;
		mat->GetTexture(aiTextureType_AMBIENT, 0, &str);
		textureAtlas.insert(std::pair<std::string, GLuint>(std::string(str.C_Str()), tex));
	}
	if (GLuint tex = res->getTexDiffuse()) {
		aiString str;
		mat->GetTexture(aiTextureType_DIFFUSE, 0, &str);
		textureAtlas.insert(std::pair<std::string, GLuint>(std::string(str.C_Str()), tex));
	}
	if (GLuint tex = res->getTexSpecular()) {
		aiString str;
		mat->GetTexture(aiTextureType_SPECULAR, 0, &str);
		textureAtlas.insert(std::pair<std::string, GLuint>(std::string(str.C_Str()), tex));
	}
	if (GLuint tex = res->getMapNormal()) {
		aiString str;
		mat->GetTexture(aiTextureType_NORMALS, 0, &str);
		textureAtlas.insert(std::pair<std::string, GLuint>(std::string(str.C_Str()), tex));
	}
	if (GLuint tex = res->getMapDisplacement()) {
		aiString str;
		mat->GetTexture(aiTextureType_HEIGHT, 0, &str);
		textureAtlas.insert(std::pair<std::string, GLuint>(std::string(str.C_Str()), tex));
	}
}
