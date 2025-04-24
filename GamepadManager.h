#pragma once

#include <vector>
#include <iostream>
#include <GLFW/glfw3.h>


class GamepadManager {
public:
	bool isConnected(int jid) const;
	void printAllJoysticks() const;
private:
};

