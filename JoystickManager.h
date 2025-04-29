#pragma once

#include<iostream>
#include<GLFW/glfw3.h>
#include<memory>
#include<vector>
#include <unordered_map>

#include"Gamepad.h"

class JoystickManager {
public:
	static bool isConnected(int jid);
	static void printAllJoysticks();
	static std::shared_ptr<Gamepad> getOrCreateGamepad(int jid);
	static std::vector<std::shared_ptr<Gamepad>> findAllAvailableGamepads();
private:
	static std::unordered_map<int, std::shared_ptr<Gamepad>> gamepads;
	// remplaccer le int par le getjid()
};

