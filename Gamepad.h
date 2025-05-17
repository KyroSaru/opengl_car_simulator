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
	// Bouton X pour switch de caméra
	bool isXPressed() const;

	// -----------------------

	// Assigne/Désassigne le clavier (unicité)
	void Assign() { gamepadAssigned = true; }
	void Unassign() { gamepadAssigned = false; }
	// Retourne si la manette est assignée
	bool isAssigned() { return gamepadAssigned; }

private:
	// ID de la manette
	int gamepadID;

	// Flag pour savoir si la manette est assigné
	bool gamepadAssigned = false;
	// Flags pour savoir si le bouton Y et X ont été pressé (pour éviter de les détecter plusieurs fois)
	mutable bool lastYPressed = false;
	mutable bool lastXPressed = false;

	// Deadzone de la manette (pour éviter les mouvements parasites)
	float deadzone = 0.2f;
};

#endif // GAMEPAD_H
