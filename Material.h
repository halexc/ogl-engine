#pragma once
#include "Shader.h"

#include <glm/glm.hpp>
#include <glad\glad.h>

#include "assimp\Importer.hpp"
#include "assimp\scene.h"
#include "assimp\postprocess.h"

#include <string>

class Material
{
public:
	// Basic Material.
	Material();
	Material(aiMaterial * mat);
	Material(aiMaterial * mat, bool ambientFromDiffuse);
	Material(std::string name, aiMaterial * mat);
	Material(std::string name, aiMaterial * mat, bool ambientFromDiffuse);
	~Material();

	// Create default textures to use when no texture is specified in the material.
	static void setupDefaultTextures();
	static void cleanupDefaultTextures();

	// Returns a white 1x1 texture.
	static GLuint getDefaultTexture();
	// Returns a light blueish (0.5, 0.5, 1.0) 1x1 texture
	static GLuint getDefaultNormalMap();
	// Returns a black 1x1 texture;
	static GLuint getDefaultHeightMap();

	// Load the data from an aiMaterial. If useDiffuseForAmbient = true, the diffuse
	// texture is also used as the ambient texture if there is no other texture.
	virtual void loadFromAiMaterial(aiMaterial * mat, bool useDiffuseForAmbient = true);
	// Load the data from an aiMaterial. Always uses the diffuse values for ambient, even
	// when there are different values in place.
	virtual void loadFromAiMaterial_wAmbientFromDiffuse(aiMaterial * mat);

	// Prepares the shader for drawing by loading the material data. 
	virtual void prepare();
	// Prepares the shader for drawing by loading the material data. Instead of using the
	// material's standard shader however, the specified shader is used.
	virtual void prepare(Shader * shader);

	// Set the ambient color (RGB-vector)
	void setColorAmbient(glm::fvec3 color);
	// Set the ambient color (3x float)
	void setColorAmbient(float r, float g, float b);
	// Set the diffuse color (RGB-vector)
	void setColorDiffuse(glm::fvec3 color);
	// Set the diffuse color (3x float)
	void setColorDiffuse(float r, float g, float b);
	// Set the specular color (RGB-vector)
	void setColorSpecular(glm::fvec3 color);
	// Set the specular color (3x float)
	void setColorSpecular(float r, float g, float b);

	// Set the Ambient texture
	void setTexAmbient(GLuint tex);
	// Set the Diffuse texture
	void setTexDiffuse(GLuint tex);
	// Set the Specular texture
	void setTexSpecular(GLuint tex);
	// Set the Normal Map texture for Normal Mapping
	void setMapNormal(GLuint map);
	// Set the Displacement Map texture for Parallax Mapping
	void setMapDisplacement(GLuint map);

	// Get the Ambient texture
	GLuint getTexAmbient();
	// Get the Diffuse texture
	GLuint getTexDiffuse();
	// Get the Specular texture
	GLuint getTexSpecular();
	// Get the Normal Map texture for Normal Mapping
	GLuint getMapNormal();
	// Get the Displacement Map texture for Parallax Mapping
	GLuint getMapDisplacement();

	void setName(const std::string name);
	const std::string getName();

	// Use this method ONLY IF this material was NOT created from a MaterialManager!
	// This deletes all textures in use freeing up the graphics memory, meaning other 
	// materials can no longer access them.
	void deleteResources();

	/*
		Set the shader program. The Shader should support 5 2D samplers,
		of which 3 are for coloring and texturing, while the 2 others are
		for normal and parallax mapping. These textures should be in a 
		'Material'-struct, named 'texAmbient', 'texDiffuse', and 'texSpecular,
		and 'normalMap' and 'parallaxMap', respectively. This is also the order in which
		The corresponding uniform
		is named 'mat'. Also, the material struct should contain booleans 'normalMapping'
		and 'parallaxMapping' in order to disable such routines in case not needed.
	*/
	void setShader(Shader * s);
	// Get the material shader
	Shader * getShader();

private:
	std::string name;

	glm::fvec3 colorAmbient;
	glm::fvec3 colorDiffuse;
	glm::fvec3 colorSpecular;

	float intensitySpecular;

	GLuint texAmbient;
	GLuint texDiffuse;
	GLuint texSpecular;
	GLuint mapNormal;
	GLuint mapDisplacement;

	Shader * shader;
};

