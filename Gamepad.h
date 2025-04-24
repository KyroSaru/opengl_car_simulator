#pragma once

#include<GLFW/glfw3.h>
#include<cstdlib>

class Gamepad {
public:
	Gamepad(int jid);
	// On va utiliser la gachette droite pour gérer l'accéleration
	float getRightTrigger() const;
	// On va utiliser la gachette gauche pour gérer déceleration
	float getLeftTrigger() const;
	// On va utiliser l'axe horizontal du stick gauche pour gérer la direction des roues
	float getLeftStickX() const;
	// On va utiliser le stick droit pour gérer la caméra
	// à implémenter plus tard (gérer le cas ou il y a plusieurs joueurs)
	float getRightStickX() const { return 0.0f; }
	float getRightStickY() const { return 0.0f; }
private:
	int _jid;
};
