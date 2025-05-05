#include"JoystickManager.h"

std::unordered_map<int, std::shared_ptr<Gamepad>> JoystickManager::gamepads;

bool JoystickManager::isConnected(int jid) {
	if (glfwJoystickIsGamepad(jid)) {
		return true;
	}
	return false;
}

void JoystickManager::printAllJoysticks()  {
	for (int jid = GLFW_JOYSTICK_1; jid <= GLFW_JOYSTICK_LAST; ++jid) {
		if(glfwJoystickPresent(jid))
			std::cout << "Joystick n°" << jid + 1 << " is connected ! Joystick name: " << glfwGetJoystickName(jid) << std::endl;
	}
}

std::shared_ptr<Gamepad> JoystickManager::getOrCreateGamepad(int jid)
{
	// On cherche si on a déjà créé ce Gamepad
	auto it = gamepads.find(jid);
	if (it != gamepads.end())
		return it->second; 

	auto newGamepad = std::make_shared<Gamepad>(jid);
	gamepads[jid] = newGamepad;
	return newGamepad;
}

std::vector<std::shared_ptr<Gamepad>> JoystickManager::findAllAvailableGamepads() {
	std::vector<std::shared_ptr<Gamepad>> availableGamepads;

	for (int jid = GLFW_JOYSTICK_1; jid <= GLFW_JOYSTICK_LAST; ++jid) {
		if (glfwJoystickIsGamepad(jid)) {
			auto gamepad = JoystickManager::getOrCreateGamepad(jid);

			if (!gamepad->isAssigned()) {
				//std::cout << "Manette n°" << jid << "detectée ; Nom de la manette : " << glfwGetJoystickName(jid) << std::endl;
				availableGamepads.push_back(gamepad);
			}
		}
	}
	return availableGamepads;
}