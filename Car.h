#ifndef CAR_H
#define CAR_H

#include "Model.h"
#include "Frustum.h"
#include "Terrain.h"
#include "Shader.h"

#include "Keyboard.h"
#include "Gamepad.h"

class Car
{
public:
	Car(const std::string& modelPath);

	// Getters/Setters
	int getId() const { return id; }
	glm::vec3 getPosition() const;
	void setPosition(const glm::vec3& position);
	glm::vec3 getDirection() const;
	void setDirection(const glm::vec3& direction);
	bool getHeadlightsOn() const { return headlightsOn; }
	void setHeadlightsOn(bool on) { headlightsOn = on; }

	// Position du corps de la voiture (ajout de la hauteur du corps par rapport aux roues)
	glm::vec3 getBodyPosition() const;

	// Model Matrix de la voiture
	glm::mat4 getCarModelMatrix() const;
	// Model Matrix du corps a utilisé dans plusieurs méthodes (Draw, calcul de bounding box...)
	glm::mat4 getBodyModelMatrix() const;

	// Calcul les angles de rotation (pitch et roll) & la hauteur du corps de la voiture
	glm::vec2 calculateBodyAngles() const;
	float calculateBodyHeight() const;

	// Vérifie si la voiture est visible dans le frustum
	bool isVisible(const Frustum& frustum) const;

	// Dessine la voiture
	void Draw(Shader& carShader, Shader& phareShader);
	// Dessine la bounding box de la voiture
	void DrawWireframes(Shader& wireframeShader, const glm::mat4& view, const glm::mat4& projection) const;

	// -----------------------

	// Fonction manette
	std::shared_ptr<Gamepad> getGamepad() { return _gamepad; }
	void setGamepad(std::shared_ptr<Gamepad> g) { _gamepad = g; }
	bool hasGamepad() const { return _gamepad != nullptr; }
	// Fonction clavier
	void setKeyboard(std::shared_ptr<Keyboard> k) { _keyboard = k; }
	bool hasKeyboard() const { return _keyboard != nullptr; }
	// Fonction caméra
	std::shared_ptr<Camera> getCamera() { return camera; }
	void setCamera(std::shared_ptr<Camera> cam) { camera = cam; }

	// -----------------------

	// Gére la physique de la voiture avec des contrôles
	void updatePhysics(float deltaTime, GLFWwindow* window, const Terrain& terrain);
	// Gestion des collisions avec les autres voitures
	bool checkCollision(const Car& other) const;
private:
	// Gestion de l'ID du véhicule
	int id;
	static int nextId;

	// Séparation du véhicule en partie (pour animation des roues et du corps séparée)
	Model body;
	Model wheels[4];
	Model fenetres;
	Model phares;
	Model retro_miroir_G, retro_miroir_D;
	Model volant;

	// Positions locales des roues par rapport au centre de la voiture
	glm::vec3 wheelOffsets[4];
	// Hauteur y des roues
	float wheelHeights[4];
	// Distance constante entre le corps et les roues
	const float bodyToWheelDistance = 0.54f; 

	// Phares
	bool headlightsOn = false;
	glm::vec3 headlightColorOn = glm::vec3(1.0f, 1.0f, 0.8f);
	glm::vec3 headlightColorOff = glm::vec3(0.5f, 0.2f, 0.2f);

	// Gestion des manettes (en dehors de la classe)
	std::shared_ptr<Gamepad> _gamepad = nullptr;
	// Gestion du clavier
	std::shared_ptr<Keyboard> _keyboard = nullptr;
	// Gestion de la caméra
	std::shared_ptr<Camera> camera = nullptr;
	
	// [ATTRIBUTS PHYSIQUE]
	glm::vec3 position;
	glm::vec3 direction;
	// Vitesse
	float currentSpeed = 0.0f;
	float maxSpeed = 15.0f;
	float acceleration = 10.0f;
	float deceleration = 5.0f;
	// Angle de braquage des roues avant (en degrés)
	float steeringAngle = 0.0f;
	const float maxSteeringAngle = 30.0f;
	// Rotation des roues
	float wheelRotationAngle = 0.0f;
	float wheelRadius = 0.5f;
};

#endif CAR_H