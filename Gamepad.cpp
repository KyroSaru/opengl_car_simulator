#include"Gamepad.h"

Gamepad::Gamepad(int jid) :
	_jid(jid) {

}

float Gamepad::getRightTrigger() const {
	int axisCount;
	const float* axes = glfwGetJoystickAxes(_jid, &axisCount);
	if (axisCount > GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER) {
		float val = axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER]; // valeur dans [-1, 1]
		return (val + 1.0f) * 0.5f; // valeur dans [0, 1]
	}
	return 0.0f;
}

float Gamepad::getLeftTrigger() const {
	int axisCount;
	const float* axes = glfwGetJoystickAxes(_jid, &axisCount);
	if (axisCount > GLFW_GAMEPAD_AXIS_LEFT_TRIGGER) {
		float val = axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER]; 
		return (val + 1.0f) * 0.5f; 
	}
	return 0.0f;
}

float Gamepad::getLeftStickX() const {
	int axisCount;
	const float* axes = glfwGetJoystickAxes(_jid, &axisCount);
	if (axisCount > GLFW_GAMEPAD_AXIS_LEFT_X) {
		float val = axes[GLFW_GAMEPAD_AXIS_LEFT_X]; // [-1, 1]
		float deadzone = 0.2f;
		return (std::abs(val) > deadzone) ? val : 0.0f; // valeur absolue obligatoire pour la partie gauche [-1, 0[
	}
	return 0.0f;
}