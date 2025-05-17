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
#include "Terrain.h"
#include "Gamepad.h"

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
	// Gére les entrées de la caméra (prend en compte deltaTime pour la sensibilité manette)
	void Inputs(GLFWwindow* window, float deltaTime);

	void setGamepad(std::shared_ptr<Gamepad> g) { gamepad = g; }

	glm::vec3 getPosition() const { return Position; }

	// Gestion des modes de caméra
	void setMode(CameraMode mode) { this->mode = mode; }
	CameraMode getMode() const { return mode; }

	// Calculer la position et la direction de la caméra en fonction de celles d'une cible à atteindre
	void followTarget(const glm::vec3& targetPosition, const glm::vec3& targetDirection, const Terrain& terrain);

	// Met à jour la caméra en fonction de son mode et de la position et direction ciblé de la voiture
	void Update(const glm::vec3& targetPosition, const glm::vec3& targetDirection, const Terrain& terrain);
private:
	// Stocke les 3 vecteurs principaux de la caméra
	glm::vec3 Position;
	glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
	// Stocke la largeur et la hauteur de la fenêtre
	int width, height;
	// Ajuster la vitesse de la caméra et la sensibilité quand on regarde autour (a voir si on peut pas faire par rapport au taux de rafraichissement de l'écran pour la sensi. car bien souvent, c'est trop rapide ou trop lent en changeant de PC)
	float speed = 15.0f;
	float sensitivity = 45.0f;
	float gamepadSensitivity = 75.0f;

	// flags : évite que la caméra ne saute au 1er clic gauche & sait si on vient de passer à la 3ème pers.
	bool firstClick = true;
	bool justSwitchedToThirdPerson = true;

	// Mode de base de la caméra
	CameraMode mode = NO_CLIP;

	// Variables pour la caméra en 3ème personne
	float distanceToTarget = 15.0f;
	float yaw = 0.0f;   // Rotation horizontale
	float pitch = 0.0f; // Rotation verticale

	// Gamepad
	std::shared_ptr<Gamepad> gamepad = nullptr;
};

#endif // CAMERA_H