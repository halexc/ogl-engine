#include "InputHandler.h"



InputHandler::InputHandler()
{
	actionKeyboard = new char[GLFW_KEY_LAST + 1];
	for (int i = 0; i <= GLFW_KEY_LAST; i++) {
		actionKeyboard[i] = (char)0x00;
	}
	actionMouse = new char[GLFW_MOUSE_BUTTON_LAST + 1];
	for (int i = 0; i <= GLFW_MOUSE_BUTTON_LAST; i++) {
		actionMouse[i] = (char)0x00;
	}
}

InputHandler::~InputHandler()
{
	delete actionKeyboard;
	delete actionMouse;
}

void InputHandler::update()
{
	for (int i = 0; i <= GLFW_KEY_LAST; i++) {
		actionKeyboard[i] &= INPUT_HOLD;
	}
	for (int i = 0; i <= GLFW_MOUSE_BUTTON_LAST; i++) {
		actionMouse[i] &= INPUT_HOLD;
	}
}

void InputHandler::keyboardCallbackFunc(GLFWwindow * window, int key, int scancode, int action, int mods)
{
	actionKeyboard[key] &= INPUT_HOLD;
	if (action == GLFW_PRESS) {
		actionKeyboard[key] = INPUT_PRESSED | INPUT_HOLD;
	}
	if (action == GLFW_REPEAT) {
		actionKeyboard[key] |= INPUT_REPEAT;
	}
	if (action == GLFW_RELEASE) {
		actionKeyboard[key] = INPUT_RELEASE;
	}
	setModifiers(mods);

	if (callbackKeyboard) callbackKeyboard(window, key, scancode, action, mods);
}

void InputHandler::mouseCallbackFunc(GLFWwindow * window, int button, int action, int mods)
{
	actionMouse[button] &= INPUT_HOLD;
	if (action == GLFW_PRESS) {
		actionMouse[button] = INPUT_PRESSED | INPUT_HOLD;
	}
	if (action == GLFW_RELEASE) {
		actionMouse[button] = INPUT_RELEASE;
	}
	setModifiers(mods);

	if (callbackMouse) callbackMouse(window, button, action, mods);
}

void InputHandler::updateMouseData(GLFWwindow * window)
{
	double x, y;
	glfwGetCursorPos(window, &x, &y);
	deltaX = x - xpos;
	deltaY = y - ypos;
	xpos = x;
	ypos = y;
}

void InputHandler::getMousePosition(double & x, double & y)
{
	x = this->xpos;
	y = this->ypos;
}

void InputHandler::getMouseDelta(double & dx, double & dy)
{
	dx = this->deltaX;
	dy = this->deltaY;
}

char InputHandler::getKeyState(int key)
{
	return actionKeyboard[key];
}

char InputHandler::getMouseButtonState(int mouseButton)
{
	return actionMouse[mouseButton];
}

char InputHandler::getModifiers()
{
	return 0;
}

bool InputHandler::isShiftDown()
{
	return (bool)(modifier & GLFW_MOD_SHIFT);
}

bool InputHandler::isCtrlDown()
{
	return (bool)(modifier & GLFW_MOD_CONTROL);
}

bool InputHandler::isAltDown()
{
	return (bool)(modifier & GLFW_MOD_ALT);
}

bool InputHandler::isCapslockActive()
{
	return (bool)(modifier & GLFW_MOD_CAPS_LOCK);
}

void InputHandler::setKeyboardCallback(std::function<void(GLFWwindow*, int, int, int, int)> func)
{
	callbackKeyboard = func;
}

void InputHandler::setMouseCallback(std::function<void(GLFWwindow*, int, int, int)> func)
{
	callbackMouse = func;
}

void InputHandler::setCursorCallback(std::function<void(GLFWwindow*, double, double)> func)
{
	callbackCursor = func;
}

inline void InputHandler::setModifiers(int mods)
{
	modifier = (char)(mods);
}
