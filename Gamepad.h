#pragma once

#include<GLFW/glfw3.h>
#include<cstdlib>

class Gamepad {
public:
	Gamepad(int jid);

	int jid() const;

	// Gachette droite pour g�rer acc�l�ration
	float getRightTrigger() const;
	// Gachette gauche pour g�rer d�c�l�ration
	float getLeftTrigger() const;
	// Stick gauche pour g�rer la direction des roues (axe horizontal)
	float getLeftStickX() const;
	// Stick droit pour g�rer la cam�ra
	float getRightStickX() const;
	float getRightStickY() const;
	// Bouton Y pour allumer les phares
	bool isYPressed() const;
	bool wasYPressed() const;

	// Assigner/D�sassigner la manette (pour l'unicit�)
	void assign() { assigned = true; }
	void unassign() { assigned = false; }
	bool isAssigned() { return assigned; }

private:
	int _jid;
	bool assigned = false;

	mutable bool lastYPressed = false;
};
