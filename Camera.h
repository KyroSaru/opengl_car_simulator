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

enum CameraMode {
	NO_CLIP,
	THIRD_PERSON
};

class Camera
{
public:
	// Constructeur de la cam�ra pour initialiser les valeurs
	Camera(int width, int height, glm::vec3 position);

	// Donne la matrice vue de la cam�ra
	glm::mat4 getViewMatrix();
	// G�re les entr�es de la cam�ra
	void Inputs(GLFWwindow* window);

	// Gestion des modes de cam�ra
	void setMode(CameraMode mode) { this->mode = mode; }
	CameraMode getMode() const { return mode; }

	// Calculer la position et la direction de la cam�ra en fonction de celles d'une cible � atteindre
	void followTarget(const glm::vec3& targetPosition, const glm::vec3& targetDirection);

	// Met � jour la cam�ra en fonction de son mode et de la position et direction cibl� de la voiture
	void update(const glm::vec3& targetPosition, const glm::vec3& targetDirection);
private:
	// Stocke les 3 vecteurs principaux de la cam�ra
	glm::vec3 Position;
	glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
	// Stocke la largeur et la hauteur de la fen�tre
	int width, height;
	// Ajuster la vitesse de la cam�ra et la sensibilit� quand on regarde autour
	float speed = 0.1f;
	float sensitivity = 50.0f;

	// flags : �vite que la cam�ra ne saute au 1er clic gauche & sait si on vient de passer � la 3�me pers.
	bool firstClick = true;
	bool justSwitchedToThirdPerson = true;

	// Mode de base de la cam�ra
	CameraMode mode = NO_CLIP;

	// Variables pour la cam�ra en 3�me personne
	float distanceToTarget = 15.0f;
	float yaw;   // Rotation horizontale
	float pitch; // Rotation verticale
};

#endif // CAMERA_H