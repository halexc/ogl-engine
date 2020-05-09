#include "Scene.h"

Scene::Scene()
{
	cameras.push_back(new Camera());
	matManager = new MaterialManager();
	activeCamera = 0;
}

Scene::Scene(const aiScene * scene)
{
	loadScene(scene);
}

Scene::Scene(const aiScene * scene, bool ambientFromDiffuse)
{
	loadScene(scene, ambientFromDiffuse);
}


Scene::~Scene()
{
	models.clear();
	cameras.clear();
	lights.clear();
	delete matManager;
}

void Scene::loadScene(const aiScene * scene)
{
	// Load cameras:
	if (scene->HasCameras())
		for (unsigned int i = 0; i < scene->mNumCameras; i++)
		{
			cameras.push_back(loadCamera(scene->mCameras[i]));
		}
	else {
		cameras.push_back(new Camera());
	}
	activeCamera = 0;

	// Load materials:
	if (!matManager) {
		matManager = new MaterialManager();
	}
	if (scene->HasMaterials()) {
		for (unsigned int i = 0; i < scene->mNumMaterials; i++)
		{
			matManager->loadFromAiMaterial(scene->mMaterials[i]);
		}
	}

	// Load meshes:
	if (scene->HasMeshes()) {
		for (unsigned int i = 0; i < scene->mNumMeshes; i++)
		{
			Material * mat = matManager->getMaterial("default");
			if (scene->HasMaterials())
				 mat = matManager->getMaterial(scene->mMaterials[scene->mMeshes[i]->mMaterialIndex]->GetName().C_Str());

			models.push_back(new PolygonModel(scene->mMeshes[i], mat));
		}
	}
	processNode(scene->mRootNode);

}

void Scene::loadScene(const aiScene * scene, bool ambientFromDiffuse)
{
	if (!ambientFromDiffuse) {
		loadScene(scene);
		return;
	}
	// Load cameras:
	if (scene->HasCameras())
		for (unsigned int i = 0; i < scene->mNumCameras; i++)
		{
			cameras.push_back(loadCamera(scene->mCameras[i]));
		}
	else {
		cameras.push_back(new Camera());
	}
	activeCamera = 0;

	// Load materials:
	if (!matManager) {
		matManager = new MaterialManager();
	}
	if (scene->HasMaterials()) {
		for (unsigned int i = 0; i < scene->mNumMaterials; i++)
		{
			matManager->loadFromAiMaterial(scene->mMaterials[i], true);
		}
	}

	// Load meshes:
	if (scene->HasMeshes()) {
		for (unsigned int i = 0; i < scene->mNumMeshes; i++)
		{
			Material * mat = matManager->getMaterial("default");
			if (scene->HasMaterials())
				mat = matManager->getMaterial(scene->mMaterials[scene->mMeshes[i]->mMaterialIndex]->GetName().C_Str());

			PolygonModel * model = new PolygonModel(scene->mMeshes[i], mat);
			models.push_back(model);
		}
	}
	processNode(scene->mRootNode);
}

MaterialManager * Scene::getMaterialManager()
{
	return matManager;
}

PolygonModel * Scene::getMesh(unsigned int i)
{
	return models.at(i);
}

Camera * Scene::getCamera(unsigned int i)
{
	return cameras.at(i);
}

unsigned int Scene::addCamera(Camera * c)
{
	cameras.push_back(c);
	return(cameras.size() - 1);
}

unsigned int Scene::addMesh(PolygonModel * mesh)
{
	models.push_back(mesh);
	return(models.size() - 1);
}

unsigned int Scene::addLight(Light * light)
{
	lights.push_back(light);
	return (lights.size() - 1);
}

void Scene::draw()
{
	matManager->updateShaders(cameras[activeCamera]);

	for (std::map<std::string, Shader *>::iterator it = matManager->getShaders()->begin(); it != matManager->getShaders()->end(); it++) {
		for (unsigned int i = 0; i < lights.size(); i++) {
			lights[i]->configureShader((*it).second, i);
		}
		(*it).second->setInt("nrLights", lights.size());
	}

	for (PolygonModel * mesh : models) {
		mesh->draw();
	}
}

void Scene::processNode(aiNode * node, glm::fmat4 transformParent)
{
	PolygonModel * model = NULL;
	glm::fmat4 transformNode;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			transformNode[i][j] = node->mTransformation[j][i];
		}
	}

	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		model = models[node->mMeshes[i]];
		model->getTransform()->setTransformGlobal(transformParent * transformNode);
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		if (model) processNode(node->mChildren[i], model->getTransform());
		else processNode(node->mChildren[i], transformParent * transformNode);
	}
}

void Scene::processNode(aiNode * node, Transform3D * parent)
{
	PolygonModel * model = NULL;
	glm::fmat4 transformNode;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			transformNode[i][j] = node->mTransformation[j][i];
		}
	}

	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		model = models[i];
		model->getTransform()->setParent(parent);
		model->getTransform()->setTransform(transformNode);
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		if (model) processNode(node->mChildren[i], model->getTransform());
		else processNode(node->mChildren[i], parent->getTransform() * transformNode);
	}
}

Camera * Scene::loadCamera(aiCamera * cam)
{
	aiVector3D camUp = cam->mUp;
	glm::fvec3 upVector = glm::fvec3(camUp.x, camUp.y, camUp.z);
	aiVector3D camPos = cam->mPosition;
	glm::fvec3 posVector = glm::fvec3(camPos.x, camPos.y, camPos.z);
	aiVector3D camLookAt = cam->mLookAt;
	glm::fvec3 tgtVector = glm::fvec3(camLookAt.x, camLookAt.y, camLookAt.z);
	float camFoV = cam->mHorizontalFOV;
	float camAR = cam->mAspect;
	float zNear = cam->mClipPlaneNear;
	float zFar = cam->mClipPlaneFar;

	return new Camera(glm::perspective(camFoV, camAR, zNear, zFar), glm::lookAt(posVector, tgtVector, upVector));
}
