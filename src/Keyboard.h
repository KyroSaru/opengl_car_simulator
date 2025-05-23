#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <GLFW/glfw3.h>

class Keyboard 
{
public:
	// Constructeur par défaut
	Keyboard() = default;

	// Pour savoir si une touche est pressée
	bool isForwardPressed(GLFWwindow* window) const { return glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS; }
	bool isBackwardPressed(GLFWwindow* window) const { return glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS; }
	bool isLeftPressed(GLFWwindow* window) const { return glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS; }
	bool isRightPressed(GLFWwindow* window) const { return glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS; }

	// Pour savoir si une touche a été pressée (et éviter que ça la spam / binaire)
	bool wasEnterPressed(GLFWwindow* window) const;
	bool wasBackspacePressed(GLFWwindow* window) const;
	bool wasHPressed(GLFWwindow* window) const;
	bool wasWPressed(GLFWwindow* window) const;
	bool wasF1Pressed(GLFWwindow* window) const;

	// Augmente/Diminue le facteur d'accél. et l'angle de braquage de Car
	float getSteerInput(GLFWwindow* window) const;
	float getAccelInput(GLFWwindow* window) const;

	// -----------------

	// Assigne/Désassigne le clavier
	void Assign() { keyboardAssigned = true; }
	void Unassign() { keyboardAssigned = false; }
	// Retourne si le clavier est assigné
	bool isAssigned() const { return keyboardAssigned; }
private:
	// Flag pour savoir si le clavier est assigné
	bool keyboardAssigned = false;
};

#endif // KEYBOARD_H