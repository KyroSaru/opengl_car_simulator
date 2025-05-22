#ifndef JOYSTICKMANAGER_H
#define JOYSTICKMANAGER_H

#include<iostream>
#include<GLFW/glfw3.h>
#include<memory>
#include<vector>

#include"Gamepad.h"

class JoystickManager {
public:
	// Initialise le callaback glfw pour les Joystick afin de les gérer dynamiquement
	static void init();

	// Ajoute/Enlève une manette de la liste
	static void addGamepad(int gamepadID);
	static void removeGamepad(int gamepadID);

	// Debug/Log
	static bool isConnected(int gamepadID);
	static void printAllJoysticks();

	// Liste de toutes les manettes disponibles
	static std::vector<std::shared_ptr<Gamepad>> findAllAvailableGamepads();
private:
	// Liste des manettes
	static std::vector<std::shared_ptr<Gamepad>> gamepads;
};

#endif // JOYSTICKMANAGER_H