#include"JoystickManager.h"

bool JoystickManager::isConnected(int jid) const {
	if (glfwJoystickIsGamepad(jid)) {
		std::cout << "Le Joystick n°" << jid + 1 << "est connectée !\n";
		return true;
	}
	std::cout << "La Joystick n°" << jid + 1 << "n'est pas connectée !\n";
	return false;
}

void JoystickManager::printAllJoysticks() const {
	for (int jid = GLFW_JOYSTICK_1; jid < GLFW_JOYSTICK_LAST; ++jid) {
		if(glfwJoystickPresent(jid))
			std::cout << "Le Joystick n°" << jid + 1 << "est connectée ! Nom du Joystick : " << glfwGetJoystickName(jid) << std::endl;
	}
}

std::shared_ptr<Gamepad> JoystickManager::findGamepad() const {
	for (int jid = GLFW_JOYSTICK_1; jid < GLFW_JOYSTICK_LAST; ++jid) {
		if (glfwJoystickIsGamepad(jid)) {
			std::cout << "Manette n°" << jid << "detectée ; Nom de la manette : " << glfwGetJoystickName(jid) << std::endl;
			return std::make_shared<Gamepad>(jid);
		}
	}
	return nullptr;
}
