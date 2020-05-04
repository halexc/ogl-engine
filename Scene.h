#pragma once

#include <vector>
#include <map>

#include "assimp\Importer.hpp"
#include "assimp\scene.h"
#include "assimp\postprocess.h"

#include "MaterialManager.h"
#include "PolygonModel.h"
#include "Light.h"
#include "Camera.h"

class Scene
{
public:
	Scene();
	Scene(const aiScene * scene);
	Scene(const aiScene * scene, bool ambientFromDiffuse);
	~Scene();

	void loadScene(const aiScene * scene);
	void loadScene(const aiScene * scene, bool ambientFromDiffuse);

	MaterialManager * getMaterialManager();
	PolygonModel * getMesh(unsigned int i);
	Camera * getCamera(unsigned int i);
	
	unsigned int addCamera(Camera * c);
	unsigned int addMesh(PolygonModel * mesh);
	unsigned int addLight(Light * light);

	void draw();

private:
	unsigned int activeCamera;

	std::vector<PolygonModel *> models;
	std::vector<Camera *> cameras;
	std::vector<Light *> lights;

	MaterialManager * matManager;

	Camera * loadCamera(aiCamera * cam);
};

