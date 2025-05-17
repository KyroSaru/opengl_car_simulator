#include"Gamepad.h"

Gamepad::Gamepad(int gamepadID) :
	gamepadID(gamepadID)
{
}

// -------------------

float Gamepad::getRightTrigger() const {
	int axisCount;
	const float* axes = glfwGetJoystickAxes(gamepadID, &axisCount);

	if (axisCount > GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER) {
		// Valeur comprise entre -1 et 1
		float val = axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER];
		// On garde entre 0 et 1
		return (val + 1.0f) * 0.5f;
	}

	return 0.0f;
}

float Gamepad::getLeftTrigger() const {
	int axisCount;
	const float* axes = glfwGetJoystickAxes(gamepadID, &axisCount);

	if (axisCount > GLFW_GAMEPAD_AXIS_LEFT_TRIGGER) {
		float val = axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER]; 
		return (val + 1.0f) * 0.5f; 
	}

	return 0.0f;
}

// -------------------

float Gamepad::getLeftStickX() const {
	int axisCount;
	const float* axes = glfwGetJoystickAxes(gamepadID, &axisCount);

	if (axisCount > GLFW_GAMEPAD_AXIS_LEFT_X) {
		// Valeur comprise entre -1 et 1
		float val = axes[GLFW_GAMEPAD_AXIS_LEFT_X];
		// Vérifie si la valeur est supérieure à la deadzone (on inverse val pour le stick gauche)
		return (std::abs(val) > deadzone) ? -val : 0.0f;
	}

	return 0.0f;
}

float Gamepad::getRightStickX() const {
	int axisCount;
	const float* axes = glfwGetJoystickAxes(gamepadID, &axisCount);

	if (axisCount > GLFW_GAMEPAD_AXIS_RIGHT_X) {
		float val = axes[GLFW_GAMEPAD_AXIS_RIGHT_X];
		return (std::abs(val) > deadzone) ? val : 0.0f; 
	}

	return 0.0f;
}

float Gamepad::getRightStickY() const {
	int axisCount;
	const float* axes = glfwGetJoystickAxes(gamepadID, &axisCount);
	if (axisCount > GLFW_GAMEPAD_AXIS_RIGHT_Y) {
		float val = axes[GLFW_GAMEPAD_AXIS_RIGHT_Y];
		return (std::abs(val) > deadzone) ? val : 0.0f; 
	}
	return 0.0f;
}

// -------------------

bool Gamepad::isYPressed() const {
	int buttonCount;
	const unsigned char* buttons = glfwGetJoystickButtons(gamepadID, &buttonCount);

	if (buttonCount > GLFW_GAMEPAD_BUTTON_Y) {
		bool currentYPressed = buttons[GLFW_GAMEPAD_BUTTON_Y] == GLFW_PRESS;

		// Vérifie si le bouton Y a été pressé & met à jour le flag
		bool wasPressed = !lastYPressed && currentYPressed; 
		lastYPressed = currentYPressed;

		return wasPressed;
	}
	return false;
}

bool Gamepad::isXPressed() const {
	int buttonCount;
	const unsigned char* buttons = glfwGetJoystickButtons(gamepadID, &buttonCount);

	if (buttonCount > GLFW_GAMEPAD_BUTTON_X) {
		bool currentYPressed = buttons[GLFW_GAMEPAD_BUTTON_X] == GLFW_PRESS;

		bool wasPressed = !lastXPressed && currentYPressed;
		lastXPressed = currentYPressed;

		return wasPressed;
	}
	return false;
}