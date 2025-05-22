#include"JoystickManager.h"

std::vector<std::shared_ptr<Gamepad>> JoystickManager::gamepads;

void JoystickManager::init()
{
	// Quand l'on connecte une manette, on l'ajoute à la liste et inversement
	glfwSetJoystickCallback([](int gamepadID, int event)
		{
			if (event == GLFW_CONNECTED)
				JoystickManager::addGamepad(gamepadID);
			else if (event == GLFW_DISCONNECTED)
				JoystickManager::removeGamepad(gamepadID);
		});

	// Dans le cas ou des manettes sont déjà connecté on doit manuellement les ajoutés à la liste, le gestionnaire d'évenement n'étant pas actif avant le lancement du jeu ...
	for (int gamepadID = GLFW_JOYSTICK_1; gamepadID <= GLFW_JOYSTICK_LAST; ++gamepadID) {
		if (glfwJoystickIsGamepad(gamepadID)) {
			addGamepad(gamepadID);
		}
	}
}

void JoystickManager::addGamepad(int gamepadID)
{
	if (glfwJoystickIsGamepad(gamepadID)) {
		// Si la manette est déjà dans la liste, on ne l'ajoute pas
		for (const auto& g : gamepads) {
			if (g->getGamepadID() == gamepadID) {
				return;
			}
		}

		auto gamepad = std::make_shared<Gamepad>(gamepadID);
		gamepads.push_back(gamepad);

		std::cout << "Gamepad " << gamepadID + 1 << " added to the list of usable gamepads! Gamepad name: " << glfwGetJoystickName(gamepadID) << std::endl;
	}
}

void JoystickManager::removeGamepad(int gamepadID)
{
	// Recherche la manette dans la liste
	auto it = std::remove_if(gamepads.begin(), gamepads.end(),
		[gamepadID](const std::shared_ptr<Gamepad>& g) { return g->getGamepadID() == gamepadID; });

	// Supprime la manette si trouvée
	if (it != gamepads.end()) {
		gamepads.erase(it, gamepads.end());

		std::cout << "Gamepad " << gamepadID + 1 << " removed from the list of usable gamepads!\n";
	}
}

// ------------------------------

bool JoystickManager::isConnected(int gamepadID) {
	if (glfwJoystickIsGamepad(gamepadID)) {
		return true;
	}
	return false;
}
void JoystickManager::printAllJoysticks()  {
	for (int gamepadID = GLFW_JOYSTICK_1; gamepadID <= GLFW_JOYSTICK_LAST; ++gamepadID)
	{
		if(glfwJoystickPresent(gamepadID)) std::cout << "Gamepad " << gamepadID + 1 << " is connected ! Gamepad name: " << glfwGetJoystickName(gamepadID) << std::endl;
	}
}

// ------------------------------

std::vector<std::shared_ptr<Gamepad>> JoystickManager::findAllAvailableGamepads() {
	std::vector<std::shared_ptr<Gamepad>> availableGamepads;

	for (const auto& gamepad : gamepads) {
		if (!gamepad->isAssigned())
			availableGamepads.push_back(gamepad);
	}

	return availableGamepads;
}