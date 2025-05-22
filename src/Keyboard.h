#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <GLFW/glfw3.h>

class Keyboard 
{
public:
	// Constructeur par d�faut
	Keyboard() = default;

	// Pour savoir si une touche est press�e
	bool isForwardPressed(GLFWwindow* window) const { return glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS; }
	bool isBackwardPressed(GLFWwindow* window) const { return glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS; }
	bool isLeftPressed(GLFWwindow* window) const { return glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS; }
	bool isRightPressed(GLFWwindow* window) const { return glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS; }

	// Pour savoir si une touche a �t� press�e (et �viter que �a la spam / binaire)
	bool wasEnterPressed(GLFWwindow* window) const;
	bool wasBackspacePressed(GLFWwindow* window) const;
	bool wasHPressed(GLFWwindow* window) const;
	bool wasWPressed(GLFWwindow* window) const;
	bool wasF1Pressed(GLFWwindow* window) const;

	// Augmente/Diminue le facteur d'acc�l. et l'angle de braquage de Car
	float getSteerInput(GLFWwindow* window) const;
	float getAccelInput(GLFWwindow* window) const;

	// -----------------

	// Assigne/D�sassigne le clavier
	void Assign() { keyboardAssigned = true; }
	void Unassign() { keyboardAssigned = false; }
	// Retourne si le clavier est assign�
	bool isAssigned() const { return keyboardAssigned; }
private:
	// Flag pour savoir si le clavier est assign�
	bool keyboardAssigned = false;
};

#endif // KEYBOARD_H