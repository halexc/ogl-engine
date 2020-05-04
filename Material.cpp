#include "Material.h"

#include "Utils.h"

static GLuint defaultTex, defaultNormal, defaultHeight;

Material::Material()
{
	colorAmbient = glm::fvec3(1.0f, 1.0f, 1.0f);
	colorDiffuse = glm::fvec3(1.0f, 1.0f, 1.0f);
	colorSpecular = glm::fvec3(1.0f, 1.0f, 1.0f);

	intensitySpecular = 32;

	texAmbient = 0;
	texDiffuse = 0;
	texSpecular = 0;
	mapNormal = 0;
	mapDisplacement = 0;

	shader = NULL;
}

Material::Material(aiMaterial * mat)
{
	loadFromAiMaterial(mat);
	shader = NULL;

}

Material::Material(aiMaterial * mat, bool ambientFromDiffuse)
{
	if (ambientFromDiffuse)
		loadFromAiMaterial_wAmbientFromDiffuse(mat);
	else
		loadFromAiMaterial(mat);
	shader = NULL;
}

Material::Material(std::string name, aiMaterial * mat) : Material(mat)
{
	this->name = name;
}

Material::Material(std::string name, aiMaterial * mat, bool ambientFromDiffuse) : Material(mat, ambientFromDiffuse)
{
	this->name = name;
}


Material::~Material()
{
}

void Material::setupDefaultTextures()
{
	glGenTextures(1, &defaultTex);
	GLubyte data[] = { 0xFF, 0xFF, 0xFF, 0xFF };
	glBindTexture(GL_TEXTURE_2D, defaultTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	glGenTextures(1, &defaultNormal);
	GLubyte dataN[] = { 0x80, 0x80, 0xFF, 0xFF };
	glBindTexture(GL_TEXTURE_2D, defaultNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, dataN);

	glGenTextures(1, &defaultHeight);
	GLubyte dataH[] = { 0x00, 0x00, 0x00, 0xFF };
	glBindTexture(GL_TEXTURE_2D, defaultHeight);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, dataH);
}

void Material::cleanupDefaultTextures()
{
	glDeleteTextures(1, &defaultTex);
	glDeleteTextures(1, &defaultNormal);
	glDeleteTextures(1, &defaultHeight);
}

GLuint Material::getDefaultTexture()
{
	return defaultTex;
}

GLuint Material::getDefaultNormalMap()
{
	return defaultNormal;
}

GLuint Material::getDefaultHeightMap()
{
	return defaultHeight;
}

void Material::loadFromAiMaterial(aiMaterial * mat, bool useDiffuseForAmbient)
{
	// Load colors
	aiColor3D color;
	mat->Get(AI_MATKEY_COLOR_AMBIENT, color);
	colorAmbient = glm::fvec3(color.r, color.g, color.b);
	mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
	colorDiffuse = glm::fvec3(color.r, color.g, color.b);
	mat->Get(AI_MATKEY_COLOR_SPECULAR, color);
	colorSpecular = glm::fvec3(color.r, color.g, color.b);

	mat->Get(AI_MATKEY_SHININESS, intensitySpecular);

	// Load textures
	aiString str;
	GLuint tex;
	if (mat->GetTextureCount(aiTextureType_AMBIENT)) {
		mat->GetTexture(aiTextureType_AMBIENT, 0, &str);
		std::string texPath = std::string("res\\").append(str.C_Str());
		Utils::LoadTextureFromImage(&tex, texPath.c_str());
	}
	else tex = defaultTex;
	texAmbient = tex;

	if (mat->GetTextureCount(aiTextureType_DIFFUSE)) {
		mat->GetTexture(aiTextureType_DIFFUSE, 0, &str);
		std::string texPath = std::string("res\\").append(str.C_Str());
		Utils::LoadTextureFromImage(&tex, texPath.c_str());
	}
	else tex = defaultTex;
	texDiffuse = tex;
	if (useDiffuseForAmbient && texAmbient == defaultTex) {
		texAmbient = texDiffuse;
	}

	if (mat->GetTextureCount(aiTextureType_SPECULAR)) {
		mat->GetTexture(aiTextureType_SPECULAR, 0, &str);
		std::string texPath = std::string("res\\").append(str.C_Str());
		Utils::LoadTextureFromImage(&tex, texPath.c_str());
	}
	else tex = defaultTex;
	texSpecular = tex;

	if (mat->GetTextureCount(aiTextureType_NORMALS)) {
		mat->GetTexture(aiTextureType_NORMALS, 0, &str);
		std::string texPath = std::string("res\\").append(str.C_Str());
		Utils::LoadTextureFromImage(&tex, texPath.c_str());
	}
	else tex = defaultNormal;
	mapNormal = tex;

	if (mat->GetTextureCount(aiTextureType_HEIGHT)) {
		mat->GetTexture(aiTextureType_HEIGHT, 0, &str);
		std::string texPath = std::string("res\\").append(str.C_Str());
		Utils::LoadTextureFromImage(&tex, texPath.c_str());
	}
	else tex = defaultHeight;
	mapDisplacement = tex;

	if(name.empty())
		name = mat->GetName().C_Str();
}

void Material::loadFromAiMaterial_wAmbientFromDiffuse(aiMaterial * mat)
{
	// Load colors
	aiColor3D color;
	mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
	colorAmbient = glm::fvec3(color.r, color.g, color.b);
	colorDiffuse = glm::fvec3(color.r, color.g, color.b);
	mat->Get(AI_MATKEY_COLOR_SPECULAR, color);
	colorSpecular = glm::fvec3(color.r, color.g, color.b);

	mat->Get(AI_MATKEY_SHININESS, intensitySpecular);

	// Load textures
	aiString str;
	GLuint tex;
	if (mat->GetTextureCount(aiTextureType_DIFFUSE)) {
		mat->GetTexture(aiTextureType_DIFFUSE, 0, &str);
		std::string texPath = std::string("res\\").append(str.C_Str());
		Utils::LoadTextureFromImage(&tex, texPath.c_str());
	}
	else tex = defaultTex;
	texDiffuse = tex;
	texAmbient = tex;

	if (mat->GetTextureCount(aiTextureType_SPECULAR)) {
		mat->GetTexture(aiTextureType_SPECULAR, 0, &str);
		std::string texPath = std::string("res\\").append(str.C_Str());
		Utils::LoadTextureFromImage(&tex, texPath.c_str());
	}
	else tex = defaultTex;
	texSpecular = tex;

	if (mat->GetTextureCount(aiTextureType_NORMALS)) {
		mat->GetTexture(aiTextureType_NORMALS, 0, &str);
		std::string texPath = std::string("res\\").append(str.C_Str());
		Utils::LoadTextureFromImage(&tex, texPath.c_str());
	}
	else tex = defaultNormal;
	mapNormal = tex;

	if (mat->GetTextureCount(aiTextureType_HEIGHT)) {
		mat->GetTexture(aiTextureType_HEIGHT, 0, &str);
		std::string texPath = std::string("res\\").append(str.C_Str());
		Utils::LoadTextureFromImage(&tex, texPath.c_str());
	}
	else tex = defaultHeight;
	mapDisplacement = tex;

	if (name.empty())
		name = mat->GetName().C_Str();
}

void Material::prepare()
{
	if (!shader) return;
	shader->use();

	shader->setInt("mat.texAmbient", 0);
	shader->setInt("mat.texDiffuse", 1);
	shader->setInt("mat.texSpecular", 2);
	shader->setInt("mat.normalMap", 3);
	shader->setInt("mat.parallaxMap", 4);

	shader->setVec3("mat.ambientColor", colorAmbient);
	shader->setVec3("mat.diffuseColor", colorDiffuse);
	shader->setVec3("mat.specularColor", colorSpecular);
	shader->setFloat("mat.specularIntensity", intensitySpecular);

	// Is there a normal map?
	if (mapNormal) {
		shader->setBool("mat.normalMapping", true);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, mapNormal);
	}
	else
		shader->setBool("mat.normalMapping", false);

	// Is there a displacement map?
	if (mapDisplacement) {
		shader->setBool("mat.parallaxMapping", true);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, mapDisplacement);
	}
	else
		shader->setBool("mat.parallaxMapping", false);

	// Set up texture samplers
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texAmbient);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texDiffuse);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, texSpecular);
}

void Material::setColorAmbient(glm::fvec3 color)
{
	colorAmbient = color;
}

void Material::setColorAmbient(float r, float g, float b)
{
	colorAmbient.x = r;
	colorAmbient.y = g;
	colorAmbient.z = b;
}

void Material::setColorDiffuse(glm::fvec3 color)
{
	colorDiffuse = color;
}

void Material::setColorDiffuse(float r, float g, float b)
{
	colorDiffuse.x = r;
	colorDiffuse.y = g;
	colorDiffuse.z = b;
}

void Material::setColorSpecular(glm::fvec3 color)
{
	colorSpecular = color;
}

void Material::setColorSpecular(float r, float g, float b)
{
	colorSpecular.x = r;
	colorSpecular.y = g;
	colorSpecular.z = b;
}

void Material::setTexAmbient(GLuint tex)
{
	texAmbient = tex;
}

void Material::setTexDiffuse(GLuint tex)
{
	texDiffuse = tex;
}

void Material::setTexSpecular(GLuint tex)
{
	texSpecular = tex;
}

void Material::setMapNormal(GLuint map)
{
	mapNormal = map;
}

void Material::setMapDisplacement(GLuint map)
{
	mapDisplacement = map;
}

GLuint Material::getTexAmbient()
{
	return texAmbient;
}

GLuint Material::getTexDiffuse()
{
	return texDiffuse;
}

GLuint Material::getTexSpecular()
{
	return texSpecular;
}

GLuint Material::getMapNormal()
{
	return mapNormal;
}

GLuint Material::getMapDisplacement()
{
	return mapDisplacement;
}

void Material::setName(const std::string name)
{
	this->name = name;
}

const std::string Material::getName()
{
	return name;
}

void Material::deleteResources()
{
	glDeleteTextures(1, &texAmbient);
	glDeleteTextures(1, &texDiffuse);
	glDeleteTextures(1, &texSpecular);
	glDeleteTextures(1, &mapNormal);
	glDeleteTextures(1, &mapDisplacement);
}

void Material::setShader(Shader * s)
{
	shader = s;
}

Shader * Material::getShader()
{
	return shader;
}
