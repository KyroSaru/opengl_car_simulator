#pragma once

#include<vector>
#include<iostream>
#include<GLFW/glfw3.h>
#include<memory>

#include"Gamepad.h"

class JoystickManager {
public:
	bool isConnected(int jid) const;
	void printAllJoysticks() const;
	std::shared_ptr<Gamepad> findGamepad() const;
};

