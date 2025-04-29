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

	bool isAddPlayerPressed(GLFWwindow* window) const { return glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS; }

	// Retour des inputs propre au déplacement de la voiture
	float getSteerInput(GLFWwindow* window) const;
	float getAccelInput(GLFWwindow* window) const;
private:
	bool assigned = false;
};