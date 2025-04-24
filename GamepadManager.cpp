#include"GamepadManager.h"

bool GamepadManager::isConnected(int jid) const {
	if (glfwJoystickIsGamepad(jid)) {
		std::cout << "La manette n°" << jid + 1 << "est connectée !\n";
		return true;
	}
	std::cout << "La manette n°" << jid + 1 << "n'est pas connectée !\n";
	return false;
}

void GamepadManager::printAllJoysticks() const {
	for (int jid = GLFW_JOYSTICK_1; jid < GLFW_JOYSTICK_LAST; ++jid) {
		if(glfwJoystickPresent(jid))
			std::cout << "La manette n°" << jid + 1 << "est connectée ! Nom de la manette : " << glfwGetJoystickName(jid) << std::endl;
	}
}

