#ifndef GAMEPAD_H
#define GAMEPAD_H

#include<GLFW/glfw3.h>
#include<cstdlib>

class Gamepad {
public:
	// Constructeur
	Gamepad(int gamepadID);

	// Getter pour l'ID de la manette
	int getGamepadID() const { return gamepadID; }

	// -----------------------

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
	// Bouton X pour switch de cam�ra
	bool isXPressed() const;

	// -----------------------

	// Assigne/D�sassigne le clavier (unicit�)
	void Assign() { gamepadAssigned = true; }
	void Unassign() { gamepadAssigned = false; }
	// Retourne si la manette est assign�e
	bool isAssigned() { return gamepadAssigned; }

private:
	// ID de la manette
	int gamepadID;

	// Flag pour savoir si la manette est assign�
	bool gamepadAssigned = false;
	// Flags pour savoir si le bouton Y et X ont �t� press� (pour �viter de les d�tecter plusieurs fois)
	mutable bool lastYPressed = false;
	mutable bool lastXPressed = false;

	// Deadzone de la manette (pour �viter les mouvements parasites)
	float deadzone = 0.2f;
};

#endif // GAMEPAD_H
