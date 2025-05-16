#pragma once

#include<GLFW/glfw3.h>
#include<cstdlib>

class Gamepad {
public:
	Gamepad(int jid);

	int jid() const;

	// Gachette droite pour gérer accélération
	float getRightTrigger() const;
	// Gachette gauche pour gérer décélération
	float getLeftTrigger() const;
	// Stick gauche pour gérer la direction des roues (axe horizontal)
	float getLeftStickX() const;
	// Stick droit pour gérer la caméra
	float getRightStickX() const;
	float getRightStickY() const;
	// Bouton Y pour allumer les phares
	bool isYPressed() const;
	bool wasYPressed() const;

	// Assigner/Désassigner la manette (pour l'unicité)
	void assign() { assigned = true; }
	void unassign() { assigned = false; }
	bool isAssigned() { return assigned; }

private:
	int _jid;
	bool assigned = false;

	mutable bool lastYPressed = false;
};
