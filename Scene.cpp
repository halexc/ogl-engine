#include "Scene.h"
#include "Component.h"

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

			Entity3D * entity = new Entity3D();
			entity->addComponent(new PolygonModel(scene->mMeshes[i], mat));
			models.push_back(entity);
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

			Entity3D * entity = new Entity3D();
			entity->addComponent(new PolygonModel(scene->mMeshes[i], mat));
			models.push_back(entity);
		}
	}
	processNode(scene->mRootNode);
}

MaterialManager * Scene::getMaterialManager()
{
	return matManager;
}

int Scene::getNumEntities()
{
	return models.size();
}

unsigned int Scene::addEntity3D(Entity3D * e)
{
	if (PolygonModel * m = e->getComponent<PolygonModel>()) {
		if (m->getMaterial()->getShader()) {
			matManager->addMaterial(m->getMaterial());
			m->setMaterialUnique(false);
		}
		else {
			m->setMaterial(matManager->getMaterial("default"));
		}
	}
	models.push_back(e);
	return models.size() - 1;
}

Entity3D * Scene::getEntity3D(unsigned int i)
{
	return models.at(i);
}

int Scene::getNumCameras()
{
	return cameras.size();
}

unsigned int Scene::getActiveCameraIndex()
{
	return activeCamera;
}

Camera * Scene::getCamera()
{
	return cameras.at(activeCamera);
}

Camera * Scene::getCamera(unsigned int i)
{
	if (i >= cameras.size()) return NULL;
	return cameras.at(i);
}

void Scene::setActiveCamera(unsigned int i)
{
	if (i >= cameras.size()) {
		std::cerr << "Scene.cpp: Error when trying to assign active camera. Index exceeds number of cameras." << std::endl;
		i = 0;
	}
	activeCamera = i;
}

void Scene::setActiveCamera(Camera * c)
{
	if (c == NULL) { 
		activeCamera = 0; 
		return;
	}
	activeCamera = addCamera(c);
}

unsigned int Scene::addCamera(Camera * c)
{
	if (c == NULL) return 0;
	std::vector<Camera *>::iterator it = std::find(cameras.begin(), cameras.end(), c);
	if (it != cameras.end()) {
		return std::distance(cameras.begin(), it);
	}
	cameras.push_back(c);
	return(cameras.size() - 1);
}

unsigned int Scene::addMesh(PolygonModel * mesh)
{
	Entity3D * entity = new Entity3D();
	entity->addComponent(mesh);
	models.push_back(entity);
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


	// Calculate shadows
	for (unsigned int i = 0; i < lights.size(); i++)
		if (lights[i]->castsShadows()) lights[i]->drawShadows(this, matManager->getShader("depthShader"));

	for (std::map<std::string, Shader *>::iterator it = matManager->getShaders()->begin(); it != matManager->getShaders()->end(); it++) {
		unsigned int nPointLights = 0;
		unsigned int nDirLights = 0;
		for (Light * l : lights) {

			if (dynamic_cast<PointLight*>(l)) {
				l->configureShader((*it).second, nPointLights);
				nPointLights++;
			}
			else if (dynamic_cast<DirectionalLight*>(l)) {
				l->configureShader((*it).second, nDirLights);
				nDirLights++;
			}
		}
		(*it).second->setInt("nPointLights", nPointLights);
		(*it).second->setInt("nDirLights", nDirLights);
	}

	for (Entity3D * entity : models) {
		entity->draw();
	}
}

void Scene::processNode(aiNode * node, glm::fmat4 transformParent)
{
	Entity3D * model = NULL;
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
	Entity3D * model = NULL;
	glm::fmat4 transformNode;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			transformNode[i][j] = node->mTransformation[j][i];
		}
	}

	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		model = models[i];
		model->getTransform();
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
