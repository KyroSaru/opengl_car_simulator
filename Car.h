#ifndef CAR_H
#define CAR_H

#include "Model.h"
#include "Frustum.h"

class Car
{
public:
	Car(const std::string& modelPath);

	// Getters/Setters
	glm::vec3 getPosition() const;
	glm::vec3 getDirection() const;
	void setPosition(const glm::vec3& position);
	void setDirection(const glm::vec3& direction);

	// Model Matrix du body a utilisé dans plusieurs méthodes (Draw, calcul de bounding box...)
	glm::mat4 getBodyModelMatrix() const;

	// Vérifie si la voiture est visible dans le frustum
	bool isVisible(const Frustum& frustum) const;

	// Dessine la voiture
	void Draw(Shader& shader);
	// Dessine la bounding box de la voiture
	void DrawWireframes(Shader& wireframeShader, const glm::mat4& view, const glm::mat4& projection) const;


	// -----------------------

	// Gére la physique de la voiture avec des contrôles
	void updatePhysics(float deltaTime, GLFWwindow* window);
private:
	// Séparation des parts du véhicule (pour futur animation des roues)
	Model body;
	Model wheels[4];
	// Positions locales des roues par rapport au centre de la voiture
	glm::vec3 wheelOffsets[4];

	// [ATTRIBUTS PHYSIQUE]
	glm::vec3 position;
	glm::vec3 direction;

	// Angle de braquage des roues avant (en degrés)
	float steeringAngle = 0.0f; 
	const float maxSteeringAngle = 30.0f;
	// Vitesse
	float currentSpeed = 0.0f;
	float maxSpeed = 10.0f;
	float acceleration = 10.0f;     // m/s²
	float deceleration = 5.0f;      // m/s²
	// Rotation des roues
	float wheelRotationAngle = 0.0f;
	float wheelRadius = 0.5f;
};

#endif CAR_H