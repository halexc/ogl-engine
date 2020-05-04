#pragma once

#include <GLFW\glfw3.h>

#define INPUT_PRESSED	0b0001
#define INPUT_HOLD		0b0010
#define INPUT_RELEASE	0b0100
#define INPUT_REPEAT	0b1000

#include <functional>

class InputHandler
{
public:
	InputHandler();
	~InputHandler();

	void update();
	void keyboardCallbackFunc(GLFWwindow* window, int key, int scancode, int action, int mods);
	void mouseCallbackFunc(GLFWwindow* window, int button, int action, int mods);
	void updateMouseData(GLFWwindow* window);

	void getMousePosition(double &x, double &y);
	void getMouseDelta(double &dx, double &dy);

	char getKeyState(int key);
	char getMouseButtonState(int mouseButton);

	char getModifiers();
	bool isShiftDown();
	bool isCtrlDown();
	bool isAltDown();
	bool isCapslockActive();
	
	void setKeyboardCallback(std::function<void(GLFWwindow*, int, int, int, int)> func);
	void setMouseCallback(std::function<void(GLFWwindow*, int, int, int)> func);
	void setCursorCallback(std::function<void(GLFWwindow*, double, double)> func);

private:

	std::function<void(GLFWwindow*, int, int, int, int)>	callbackKeyboard = 0;
	std::function<void(GLFWwindow*, int, int, int)>			callbackMouse = 0;
	std::function<void(GLFWwindow*, double, double)>		callbackCursor = 0;

	inline void setModifiers(int mods);

	double xpos = 0, ypos = 0, deltaX = 0, deltaY = 0;
	char modifier = 0x00;
	char * actionKeyboard;
	char * actionMouse;
};

