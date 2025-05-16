#include"JoystickManager.h"

std::vector<std::shared_ptr<Gamepad>> JoystickManager::gamepads;

void JoystickManager::init()
{
	glfwSetJoystickCallback([](int jid, int event)
		{
			if (event == GLFW_CONNECTED)
				JoystickManager::addGamepad(jid);
			else if (event == GLFW_DISCONNECTED)
				JoystickManager::removeGamepad(jid);
		});

	// Dans le cas ou des manettes sont déjà connecté on doit manuellement les ajoutés à la liste, le gestionnaire d'évenement n'étant pas actif avant le lancement du jeu ...
	for (int jid = GLFW_JOYSTICK_1; jid <= GLFW_JOYSTICK_LAST; ++jid) {
		if (glfwJoystickIsGamepad(jid)) {
			addGamepad(jid);
		}
	}
}

void JoystickManager::addGamepad(int jid) {
	if (glfwJoystickIsGamepad(jid)) {
		// Vérifie si le gamepad déjà présent dans la liste
		for (const auto& g : gamepads) {
			if (g->jid() == jid) {
				return;
			}
		}

		auto gamepad = std::make_shared<Gamepad>(jid);
		gamepads.push_back(gamepad);
		std::cout << "Le Gamepad n°" << jid + 1 << "a été ajouté à la listes de gamepads utilisables ! Nom du Gamepad : " << glfwGetJoystickName(jid) << std::endl;
	}
}

void JoystickManager::removeGamepad(int jid) {

	auto it = std::remove_if(gamepads.begin(), gamepads.end(),
		[jid](const std::shared_ptr<Gamepad>& g) { return g->jid() == jid; });

	if (it != gamepads.end()) {
		gamepads.erase(it, gamepads.end());
		std::cout << "Le Gamepad n°" << jid + 1 << "a été retiré de la listes de gamepads utilisables !\n";
	}
}


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


std::vector<std::shared_ptr<Gamepad>> JoystickManager::findAllAvailableGamepads() {
	std::vector<std::shared_ptr<Gamepad>> availableGamepads;

	for (const auto& gamepad : gamepads) {
		if (!gamepad->isAssigned())
			availableGamepads.push_back(gamepad);
	}

	return availableGamepads;
}