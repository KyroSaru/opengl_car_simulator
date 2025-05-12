#pragma once

#include<iostream>
#include<GLFW/glfw3.h>
#include<memory>
#include<vector>
#include<algorithm>

#include"Gamepad.h"

class JoystickManager {
public:
	// Initialise le callaback glfw pour les Joystick afin de les gérer dynamiquement
	static void init();

	static void addGamepad(int jid);
	static void removeGamepad(int jid);

	// Debug
	static bool isConnected(int jid);
	static void printAllJoysticks();

	static std::vector<std::shared_ptr<Gamepad>> findAllAvailableGamepads();
private:
	static std::vector<std::shared_ptr<Gamepad>> gamepads;
};
