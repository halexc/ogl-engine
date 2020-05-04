#include <glad\glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW\glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <stdlib.h>
#include <stdio.h>

#include "TriangleModel.h"
#include "Shader.h"
#include "Utils.h"
#include "Camera.h"
#include "InputHandler.h"
#include "ModifierVertexGroup.h"

#include "Scene.h"

#include "assimp\Importer.hpp"
#include "assimp\scene.h"
#include "assimp\postprocess.h"

GLFWwindow * window;
int windowWidth = 1280, windowHeight = 720;
double delta;
bool running = true;
bool showAxis;
bool showGrid;

bool cameraInit = false;

TriangleModel * model;
TriangleModel * model_Monkey;
ModifierVertexGroup * modelModifier;
InputHandler * inputHandler = new InputHandler();

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	inputHandler->keyboardCallbackFunc(window, key, scancode, action, mods);
}
void mouseCallback(GLFWwindow* window, int button, int action, int mods) {
	inputHandler->mouseCallbackFunc(window, button, action, mods);
}

void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void inputHandling();
void drawAxises(Shader s);
void drawGrid(Shader s);

Camera * c;

static void errorCallback(int error, const char * desc) {
	fprintf(stderr, "Error: %s \n", desc);
}

void setupGLFW() {
	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwSetErrorCallback(errorCallback);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);


	window = glfwCreateWindow(windowWidth, windowHeight, "Cloth Simulation 1.0", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(window, keyboardCallback);
	glfwSetMouseButtonCallback(window, mouseCallback);
	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);

	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_DEPTH_TEST);
}

int main(void) {
	setupGLFW();
	Material::setupDefaultTextures();

	Assimp::Importer importer;
	Scene * scene;
	//if (const aiScene* sn = importer.ReadFile("res\\multipleObjects_smooth.obj", aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_ValidateDataStructure)) {
	//if (const aiScene* sn = importer.ReadFile("res\\cube.obj", aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_ValidateDataStructure)) {
	if (const aiScene* sn = importer.ReadFile("res\\Pool.obj", aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_ValidateDataStructure)) {
		scene = new Scene(sn, true);
		std::cout << "File loaded." << std::endl;
	}
	else {
		std::cerr << importer.GetErrorString() << std::endl;
		return 0;
	}
	
	Light * l = new Light();
	l->setPosition(0.0f, 1.0f, 0.0f);
	l->setIntensity(0.0f);
	l->setColor(glm::fvec3(1.0f, 1.0f, 1.0f));
	scene->addLight(l);

	l = new Light();
	l->setPosition(5.0f, 0.0f, 2.5f);
	l->setIntensity(10.0f);
	l->setColor(glm::fvec3(1.0f, 1.0f, 1.0f));
	scene->addLight(l);

	l = new Light();
	l->setPosition(2.5f, 5.0f, 0.0f);
	l->setIntensity(10.0f);
	l->setColor(glm::fvec3(1.0f, 1.0f, 1.0f));
	scene->addLight(l);

	l = new Light();
	l->setPosition(0.0f, 2.5f, 5.0f);
	l->setIntensity(10.0f);
	l->setColor(glm::fvec3(1.0f, 1.0f, 1.0f));
	scene->addLight(l);
	
	double time = glfwGetTime();
	double oldTime = time;
	int fps = 0, fps_counter = 0;

	c = scene->getCamera(0);

	while (running) {
		if (glfwWindowShouldClose(window)) {
			running = false;
			break;
		}

		delta = glfwGetTime() - time;
		time = glfwGetTime();

		// Reset InputHandler state:
		inputHandler->update();
		inputHandler->updateMouseData(window);

		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		scene->draw();

		// Swap buffers:
		glfwSwapBuffers(window);

		// Poll events:
		glfwPollEvents();

		// Handle input:
		inputHandling();

		/*
		fps_counter++;
		if (time - oldTime > 1) {
			fps = fps_counter / (time - oldTime);
			std::cout << "FPS: " << fps << std::endl;
			fps_counter = 0;
			oldTime = time;

			glm::fvec3 camPos = c->getTransform()->getPosition();
			std::cout << "Position: x=" << camPos.x << ", y=" << camPos.y << ", z=" << camPos.z << std::endl;
			glm::fvec3 camVec = c->getTransform()->getRight();
			std::cout << "Right(Local X)-Vector: ( " << camVec.x << ",  " << camVec.y << ", " << camVec.z << ")" << std::endl;
			camVec = c->getTransform()->getUp();
			std::cout << "Up(Local Y)-Vector: ( " << camVec.x << ",  " << camVec.y << ", " << camVec.z << ")" << std::endl;
			camVec = c->getTransform()->getForward();
			std::cout << "Forward(Local Z)-Vector: ( " << camVec.x << ",  " << camVec.y << ", " << camVec.z << ")" << std::endl;
		}
		*/
	}

	delete scene;

	glfwDestroyWindow(window);
	glfwTerminate();
	return 1;

}


void inputHandling() {
	double d = delta;
	if (inputHandler->isShiftDown()) d = delta / 4;
	if (inputHandler->isCtrlDown()) d = delta * 2;

	if (inputHandler->getKeyState(GLFW_KEY_UP) & INPUT_HOLD || inputHandler->getKeyState(GLFW_KEY_W) & INPUT_HOLD) {
		c->getTransform()->translateOriented(glm::fvec3(0.0f, 0.0f, -float(d) * 4.0f));
	}
	if (inputHandler->getKeyState(GLFW_KEY_DOWN) & INPUT_HOLD || inputHandler->getKeyState(GLFW_KEY_S) & INPUT_HOLD) {
		c->getTransform()->translateOriented(glm::fvec3(0.0f, 0.0f, float(d) * 4.0f));
	}
	if (inputHandler->getKeyState(GLFW_KEY_LEFT) & INPUT_HOLD || inputHandler->getKeyState(GLFW_KEY_A) & INPUT_HOLD) {
		c->getTransform()->translateOriented(glm::fvec3(-float(d) * 4.0f, 0.0f, 0.0f));
	}
	if (inputHandler->getKeyState(GLFW_KEY_RIGHT) & INPUT_HOLD || inputHandler->getKeyState(GLFW_KEY_D) & INPUT_HOLD) {
		c->getTransform()->translateOriented(glm::fvec3(float(d) * 4.0f, 0.0f, 0.0f));
	}
	if (inputHandler->getKeyState(GLFW_KEY_F1) & INPUT_PRESSED) {
		showAxis = !showAxis;
		std::cout << "Toggle axis drawing." << std::endl;
	}

	if (inputHandler->getKeyState(GLFW_KEY_1) & INPUT_PRESSED) {
		c->setView(glm::lookAt(glm::vec3(0.0f, 0.5f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
	}
	if (inputHandler->getKeyState(GLFW_KEY_2) & INPUT_PRESSED) {
		c->setView(glm::lookAt(glm::vec3(0.0f, 3.25f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
	}
	if (inputHandler->getKeyState(GLFW_KEY_3) & INPUT_PRESSED) {
		c->setView(glm::lookAt(glm::vec3(3.0f, 0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
	}

	if (inputHandler->getMouseButtonState(GLFW_MOUSE_BUTTON_1) & INPUT_HOLD) {
		double dx, dy;
		inputHandler->getMouseDelta(dx, dy);
		c->getTransform()->rotate(float(-dy / windowHeight) * glm::pi<float>(), glm::fvec3(1.0f, 0.0f, 0.0f));
		c->getTransform()->rotateGlobal(float(-dx / windowWidth) * glm::pi<float>(), glm::fvec3(0.0f, 1.0f, 0.0f));
	}
	if (inputHandler->getMouseButtonState(GLFW_MOUSE_BUTTON_2) & INPUT_HOLD) {
		double dx, dy;
		inputHandler->getMouseDelta(dx, dy);
		c->getTransform()->rotateAroundGlobal(float(1.5 * dy / windowHeight) * glm::pi<float>(), c->getTransform()->getRight(), c->getTransform()->getPositionGlobal() - 3.0f * c->getTransform()->getForward());
		c->getTransform()->rotateAroundGlobal(float(1.5 * dx / windowWidth) * glm::pi<float>(), glm::fvec3(0.0f, 1.0f, 0.0f), c->getTransform()->getPositionGlobal() - 3.0f * c->getTransform()->getForward());
	}
}

void drawAxises(Shader s)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	s.setMat4("model", glm::fmat4(1.0f));
	glBegin(GL_LINES);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(25.0f, 0.0f, 0.0f);

		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 25.0f, 0.0f);

		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, 25.0f);
	glEnd();
}

void drawGrid(Shader s)
{
}
