#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "Shader.h"

class Camera
{
public:
	// Stocke les 3 vecteurs principaux de la cam�ra
	glm::vec3 Position;
	glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

	// Pour �viter que la cam�ra ne saute au 1er clic gauche
	bool firstClick = true;

	// Stocke la largeur et la hauteur de la fen�tre
	int width;
	int height;

	// Ajuster la vitesse de la cam�ra et la sensibilit� quand on regarde autour
	float speed = 0.1f;
	float sensitivity = 75.0f;

	// Constructeur de la cam�ra pour initialiser les valeurs
	Camera(int width, int height, glm::vec3 position);

	// Donne la matrice vue de la cam�ra
	glm::mat4 getViewMatrix();
	// G�re les entr�es de la cam�ra
	void Inputs(GLFWwindow* window);
};

#endif // CAMERA_H