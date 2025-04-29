#pragma once

#include<GLFW/glfw3.h>
#include<cstdlib>

class Gamepad {
public:
	Gamepad(int jid);

	int jid() const;

	// On va utiliser la gachette droite pour gérer l'accéleration
	float getRightTrigger() const;
	// On va utiliser la gachette gauche pour gérer déceleration
	float getLeftTrigger() const;
	// On va utiliser l'axe horizontal du stick gauche pour gérer la direction des roues
	float getLeftStickX() const;
	// On va utiliser le stick droit pour gérer la caméra
	float getRightStickX() const;
	float getRightStickY() const;
	// assigner/déassigner le gamepad (pour l'unicité)
	void assign() { assigned = true; }
	void unassign() { assigned = false; }
	bool isAssigned() { return assigned; }

private:
	int _jid;
	bool assigned = false;
};
