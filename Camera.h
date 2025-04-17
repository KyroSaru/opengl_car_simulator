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
	// Constructeur de la caméra pour initialiser les valeurs
	Camera(int width, int height, glm::vec3 position);

	// Donne la matrice vue de la caméra
	glm::mat4 getViewMatrix();
	// Gére les entrées de la caméra
	void Inputs(GLFWwindow* window);

	// Gestion des modes de caméra
	void setMode(CameraMode mode) { this->mode = mode; }
	CameraMode getMode() const { return mode; }

	// Calculer la position et la direction de la caméra en fonction de celles d'une cible à atteindre
	void followTarget(const glm::vec3& targetPosition, const glm::vec3& targetDirection);

	// Met à jour la caméra en fonction de son mode et de la position et direction ciblé de la voiture
	void update(const glm::vec3& targetPosition, const glm::vec3& targetDirection);
private:
	// Stocke les 3 vecteurs principaux de la caméra
	glm::vec3 Position;
	glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
	// Stocke la largeur et la hauteur de la fenêtre
	int width, height;
	// Ajuster la vitesse de la caméra et la sensibilité quand on regarde autour
	float speed = 0.1f;
	float sensitivity = 50.0f;

	// flags : évite que la caméra ne saute au 1er clic gauche & sait si on vient de passer à la 3ème pers.
	bool firstClick = true;
	bool justSwitchedToThirdPerson = true;

	// Mode de base de la caméra
	CameraMode mode = NO_CLIP;

	// Variables pour la caméra en 3ème personne
	float distanceToTarget = 15.0f;
	float yaw;   // Rotation horizontale
	float pitch; // Rotation verticale
};

#endif // CAMERA_H