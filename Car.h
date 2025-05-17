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
	// Constructeur
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

	// Fonctions manette
	std::shared_ptr<Gamepad> getGamepad() { return _gamepad; }
	void setGamepad(std::shared_ptr<Gamepad> g) { _gamepad = g; }
	bool hasGamepad() const { return _gamepad != nullptr; }
	// Fonctions clavier
	void setKeyboard(std::shared_ptr<Keyboard> k) { _keyboard = k; }
	bool hasKeyboard() const { return _keyboard != nullptr; }
	// Fonctions caméra
	std::shared_ptr<Camera> getCamera() const { return camera; }
	void setCamera(std::shared_ptr<Camera> cam) { camera = cam; }

	// -----------------------

	// Gére la physique de la voiture
	void updatePhysics(float deltaTime, GLFWwindow* window, const Terrain& terrain, const std::vector<Car>& allCars, const std::vector<BoundingBox>& staticObstacles);
	// Teste si la voiture entrerait en collision si elle se déplaçait à la position donnée
	bool wouldCollideAt(const glm::vec3& newPosition, const std::vector<Car>& cars, const std::vector<BoundingBox>& staticObstacles, int selfId) const;
private:
	// Gestion de l'ID du véhicule
	int id;
	static int nextId;

	// Séparation du véhicule en partie (pour animation des roues et du corps séparée)
	Model body;
	Model wheels[4];
	Model windows;
	Model headlights;
	Model leftRearviewMirror, rightRearviewMirror;
	Model steeringWheel;

	// Positions locales des roues par rapport au centre de la voiture
	glm::vec3 wheelOffsets[4];
	// Hauteur y des roues
	float wheelHeights[4];
	// Distance constante entre le corps et les roues
	const float bodyToWheelDistance = 0.54f; 

	// Positions locales du volant par rapport au centre de la voiture
	glm::vec3 steeringWheelOffset;

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

	// -----------------------------
	
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

	// -----------------------------

	// [PARAMETRES DE REBOND]
	float bounceFactor = 0.5f;         // Amortissement du rebond (0.5 = rebond amorti, 1.0 = rebond parfait)
	float bounceBackDistance = 0.5f;   // Distance de recul après rebond
	float minBounceSpeed = 0.5f;       // Vitesse minimale pour stopper le rebond
	float bouncePenetration = 0.7f;    // Fraction de pénétration dans l'obstacle avant rebond (0 = pile au contact, 1 = à fond dans l'obstacle)

	// Flag et normale pour décoincer la voiture
	bool isStuck = false;
	glm::vec3 stuckNormal = glm::vec3(0.0f);
};

#endif // CAR_H