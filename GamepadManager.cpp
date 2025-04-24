#include"GamepadManager.h"

bool GamepadManager::isConnected(int jid) const {
	if (glfwJoystickIsGamepad(jid)) {
		std::cout << "La manette n�" << jid + 1 << "est connect�e !\n";
		return true;
	}
	std::cout << "La manette n�" << jid + 1 << "n'est pas connect�e !\n";
	return false;
}

void GamepadManager::printAllJoysticks() const {
	for (int jid = GLFW_JOYSTICK_1; jid < GLFW_JOYSTICK_LAST; ++jid) {
		if(glfwJoystickPresent(jid))
			std::cout << "La manette n�" << jid + 1 << "est connect�e ! Nom de la manette : " << glfwGetJoystickName(jid) << std::endl;
	}
}

