#pragma once

#include<GLFW/glfw3.h>
#include<cstdlib>

class Gamepad {
public:
	Gamepad(int jid);

	int jid() const;

	// On va utiliser la gachette droite pour g�rer l'acc�leration
	float getRightTrigger() const;
	// On va utiliser la gachette gauche pour g�rer d�celeration
	float getLeftTrigger() const;
	// On va utiliser l'axe horizontal du stick gauche pour g�rer la direction des roues
	float getLeftStickX() const;
	// On va utiliser le stick droit pour g�rer la cam�ra
	float getRightStickX() const;
	float getRightStickY() const;
	// assigner/d�assigner le gamepad (pour l'unicit�)
	void assign() { assigned = true; }
	void unassign() { assigned = false; }
	bool isAssigned() { return assigned; }

private:
	int _jid;
	bool assigned = false;
};
