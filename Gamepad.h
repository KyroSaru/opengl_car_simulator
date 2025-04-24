#pragma once

#include<GLFW/glfw3.h>
#include<cstdlib>

class Gamepad {
public:
	Gamepad(int jid);
	// On va utiliser la gachette droite pour g�rer l'acc�leration
	float getRightTrigger() const;
	// On va utiliser la gachette gauche pour g�rer d�celeration
	float getLeftTrigger() const;
	// On va utiliser l'axe horizontal du stick gauche pour g�rer la direction des roues
	float getLeftStickX() const;
	// On va utiliser le stick droit pour g�rer la cam�ra
	// � impl�menter plus tard (g�rer le cas ou il y a plusieurs joueurs)
	float getRightStickX() const { return 0.0f; }
	float getRightStickY() const { return 0.0f; }
private:
	int _jid;
};
