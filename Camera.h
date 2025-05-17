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
	// Constructeur de la cam�ra pour initialiser les valeurs
	Camera(int width, int height, glm::vec3 position);

	// Donne la matrice vue de la cam�ra
	glm::mat4 getViewMatrix();
	// G�re les entr�es de la cam�ra (prend en compte deltaTime pour la sensibilit� manette)
	void Inputs(GLFWwindow* window, float deltaTime);

	void setGamepad(std::shared_ptr<Gamepad> g) { gamepad = g; }

	glm::vec3 getPosition() const { return Position; }

	// Gestion des modes de cam�ra
	void setMode(CameraMode mode) { this->mode = mode; }
	CameraMode getMode() const { return mode; }

	// Calculer la position et la direction de la cam�ra en fonction de celles d'une cible � atteindre
	void followTarget(const glm::vec3& targetPosition, const glm::vec3& targetDirection, const Terrain& terrain);

	// Met � jour la cam�ra en fonction de son mode et de la position et direction cibl� de la voiture
	void Update(const glm::vec3& targetPosition, const glm::vec3& targetDirection, const Terrain& terrain);
private:
	// Stocke les 3 vecteurs principaux de la cam�ra
	glm::vec3 Position;
	glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
	// Stocke la largeur et la hauteur de la fen�tre
	int width, height;
	// Ajuster la vitesse de la cam�ra et la sensibilit� quand on regarde autour (a voir si on peut pas faire par rapport au taux de rafraichissement de l'�cran pour la sensi. car bien souvent, c'est trop rapide ou trop lent en changeant de PC)
	float speed = 15.0f;
	float sensitivity = 45.0f;
	float gamepadSensitivity = 75.0f;

	// flags : �vite que la cam�ra ne saute au 1er clic gauche & sait si on vient de passer � la 3�me pers.
	bool firstClick = true;
	bool justSwitchedToThirdPerson = true;

	// Mode de base de la cam�ra
	CameraMode mode = NO_CLIP;

	// Variables pour la cam�ra en 3�me personne
	float distanceToTarget = 15.0f;
	float yaw = 0.0f;   // Rotation horizontale
	float pitch = 0.0f; // Rotation verticale

	// Gamepad
	std::shared_ptr<Gamepad> gamepad = nullptr;
};

#endif // CAMERA_H