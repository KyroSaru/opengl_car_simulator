#pragma once

#include <GLFW/glfw3.h>

class Keyboard 
{
public:
	Keyboard() = default;

	// Gestion de la connection du clavier
	bool isAssigned() const { return assigned; }
	void assign() { assigned = true; }
	void unassign() { assigned = false; }

	// Gestion des inputs
	bool isForwardPressed(GLFWwindow* window) const { return glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS; }
	bool isBackwardPressed(GLFWwindow* window) const { return glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS; }
	bool isLeftPressed(GLFWwindow* window) const { return glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS; }
	bool isRightPressed(GLFWwindow* window) const { return glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS; }

	bool isEnterPressed(GLFWwindow* window) const { return glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS; }
	bool isBackspacePressed(GLFWwindow* window) const { return glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_PRESS; }
	bool isEnterReleased(GLFWwindow* window) const { return glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_RELEASE; }
	bool isBackspaceReleased(GLFWwindow* window) const { return glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_RELEASE; }

	// Permet d'éviter que la fonction soit true à chaque frame si la touche est pressé
	bool wasEnterPressed(GLFWwindow* window) const;
	bool wasBackspacePressed(GLFWwindow* window) const;

	// Retour des inputs propre au déplacement de la voiture
	float getSteerInput(GLFWwindow* window) const;
	float getAccelInput(GLFWwindow* window) const;
private:
	bool assigned = false;
};