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

	// Call this method once after handling the input to reset the state. After calling this, only "HOLD"-events persist
	void update();

	// In the GLFW keyboard callback function, call this function once with its parameters passed.
	void keyboardCallbackFunc(GLFWwindow* window, int key, int scancode, int action, int mods);
	// In the GLFW mouse callback function, call this function once with its parameters passed.
	void mouseCallbackFunc(GLFWwindow* window, int button, int action, int mods);
	// Call this function once in each game loop iteration to update the mouse position
	void updateMouseData(GLFWwindow* window);

	// Use this function to get the mouse position. Pass two doubles as references for the x and y coordinates.
	void getMousePosition(double &x, double &y);
	// Use this function to get the mouse position. Pass a pointer to double for the x and y coordinates, respectively.
	// If NULL (0) is passed as either argument, the respective argument is ignored.
	void getMousePosition(double* x, double* y);
	// Use this function to get the mouse position change. Pass two doubles as references for the changes in x and y direction.
	void getMouseDelta(double &dx, double &dy);
	// Use this function to get the mouse position change. Pass a pointer to double for the changes in x and y direction, respectively.
	// If NULL (0) is passed as either argument, the respective argument is ignored.
	void getMouseDelta(double* dx, double* dy);

	// Use this function to get the state of a key. The input parameter is the GLFW identifier of the key.
	// The return value is a char with the respective bits set. Use the defined macros to mask the result accordingly.
	char getKeyState(int key);
	// Use this function to get the state of a mouse button. The input parameter is the GLFW identifier of the mouse button.
	// The return value is a char with the respective bits set. Use the defined macros to mask the result accordingly.
	char getMouseButtonState(int mouseButton);

	// Returns the current state of the modifiers. Use the GLFW macros to mask the result accordingly.
	char getModifiers();
	// Return whether the shift key was down during the last action.
	bool isShiftDown();
	// Return whether the control key was down during the last action.
	bool isCtrlDown();
	// Return whether the alt key was down during the last action.
	bool isAltDown();
	// Return whether caps lock was active during the last action.
	bool isCapslockActive();
	
	// Add a (modern) function pointer to code that should be executed whenever the keyboardCallback function is called.
	void setKeyboardCallback(std::function<void(GLFWwindow*, int, int, int, int)> func);
	// Add a (modern) function pointer to code that should be executed whenever the mouseCallback function is called.
	void setMouseCallback(std::function<void(GLFWwindow*, int, int, int)> func);

private:

	std::function<void(GLFWwindow*, int, int, int, int)>	callbackKeyboard = 0;
	std::function<void(GLFWwindow*, int, int, int)>			callbackMouse = 0;

	inline void setModifiers(int mods);

	double xpos = 0, ypos = 0, deltaX = 0, deltaY = 0;
	char modifier = 0x00;
	char * actionKeyboard;
	char * actionMouse;
};

