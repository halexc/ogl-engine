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
#include "Entity3D.h"

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

	int getNumEntities();
	unsigned int addEntity3D(Entity3D * e);
	Entity3D * getEntity3D(unsigned int i);

	int getNumCameras();
	unsigned int getActiveCameraIndex();
	Camera * getCamera();
	Camera * getCamera(unsigned int i);
	void setActiveCamera(unsigned int i);
	void setActiveCamera(Camera * c);
	
	unsigned int addCamera(Camera * c);
	unsigned int addMesh(PolygonModel * mesh);
	unsigned int addLight(Light * light);

	void draw();

private:
	unsigned int activeCamera;

	std::vector<Entity3D *> models;
	std::vector<Camera *> cameras;
	std::vector<Light *> lights;

	MaterialManager * matManager;
	void processNode(aiNode * node, glm::fmat4 transformParent = glm::fmat4(1.0f));
	void processNode(aiNode * node, Transform3D * parent);

	Camera * loadCamera(aiCamera * cam);
};

