#ifndef CAR_H
#define CAR_H

#include "Model.h"
#include "Frustum.h"
#include "Terrain.h"
#include "Shader.h"
#include "MaterialColors.h"

#include "Keyboard.h"
#include "Gamepad.h"

#include <deque>

struct TrailPoint {
	glm::vec3 position;
	float time; // Temps �coul� depuis l'ajout
};

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
	// Getters/Setters (pour UI)
	materialColors& getCarsColors() { return carsColors; }
	float& getMaxSpeed() { return maxSpeed; }
	float& getMaxSteeringAngle() { return maxSteeringAngle; }
	float& getSteeringSpeed() { return steeringSpeed; }
	float& getBodyToWheelDistance() { return bodyToWheelDistance; }
	float& getBounceFactor() { return bounceFactor; }
	bool& isWheelsTrailsEnabled() { return wheelsTrailsEnabled; }
	glm::vec3& getTrailColor() { return trailColor; }
	float& getTrailThickness() { return trailThickness; }
	float& getTrailDuration() { return trailLifetime; }

	// Position du corps de la voiture (ajout de la hauteur du corps par rapport aux roues)
	glm::vec3 getBodyPosition() const;

	// Model Matrix de la voiture
	glm::mat4 getCarModelMatrix() const;
	// Model Matrix du corps a utilis� dans plusieurs m�thodes (Draw, calcul de bounding box...)
	glm::mat4 getBodyModelMatrix() const;
	// Model Matrix des roues (angle du corps en param�tre pour �viter de le recalculer � chaque passage dans la boucle)
	glm::mat4 getWheelModelMatrix(int i, glm::vec2 bodyAngles) const;

	// Calcul les angles de rotation (pitch et roll) & la hauteur du corps de la voiture
	glm::vec2 calculateBodyAngles() const;
	float calculateBodyHeight() const;

	// V�rifie si la voiture est visible dans le frustum
	bool isVisible(const Frustum& frustum) const;

	// Dessine la voiture
	void Draw(Shader& carShader, Shader& phareShader);
	// Dessine les 4 train�es de la voiture
	void drawWheelsTrails(Shader& trailShader, const glm::mat4& view, const glm::mat4& projection) const;
	// Dessine la bounding box de la voiture
	void drawWireframes(Shader& wireframeShader, const glm::mat4& view, const glm::mat4& projection) const;
	
	// Etat du flag pour activer/d�sactiver le transfert de poids
	bool& isWeightTransferEnabled() { return weightTransferEnabled; }

	// -----------------------

	// Fonctions manette
	std::shared_ptr<Gamepad> getGamepad() { return gamepad; }
	void setGamepad(std::shared_ptr<Gamepad> g) { gamepad = g; }
	bool hasGamepad() const { return gamepad != nullptr; }
	// Fonctions clavier
	void setKeyboard(std::shared_ptr<Keyboard> k) { keyboard = k; }
	bool hasKeyboard() const { return keyboard != nullptr; }
	// Fonctions cam�ra
	std::shared_ptr<Camera> getCamera() const { return camera; }
	void setCamera(std::shared_ptr<Camera> cam) { camera = cam; }

	// -----------------------

	// G�re la physique de la voiture
	void updatePhysics(float deltaTime, GLFWwindow* window, const Terrain& terrain, const std::vector<Car>& allCars, const std::vector<BoundingBox>& staticObstacles, bool isNoClipEnabled);
	// Teste si la voiture entrerait en collision si elle se d�pla�ait � la position donn�e
	bool wouldCollideAt(const glm::vec3& newPosition, const std::vector<Car>& cars, const std::vector<BoundingBox>& staticObstacles, int selfId) const;
private:
	// Gestion de l'ID du v�hicule
	int id;
	static int nextId;

	// S�paration du v�hicule en partie (pour animation des roues et du chassis s�par�e)
	Model body;
	Model wheels[4];
	Model windows;
	Model headlights;
	Model leftRearviewMirror, rightRearviewMirror;
	Model steeringWheel;

	// Positions locales du volant par rapport au centre de la voiture (offsets calcul�s dans Blender)
	const glm::vec3 steeringWheelOffset;
	// Positions locales des roues par rapport au centre de la voiture
	const glm::vec3 wheelOffsets[4];
	// Hauteur (y) des roues
	float wheelHeights[4];
	// Offset vertical global du corps car init. trop dans le sol
	const float bodyHeightOffset = 0.5f;
	// Distance constante entre le corps et les roues
	float bodyToWheelDistance = 0.50f;
	
	// Flag pour les phares
	bool headlightsOn = false;

	// Mat�riaux de la voiture
	materialColors carsColors = { glm::vec3(0.9f, 0.8f, 0.6f), glm::vec3(0.8f, 0.6f, 0.6f) };
	glm::vec3 headlightColorOn = glm::vec3(1.0f, 1.0f, 0.8f);
	glm::vec3 headlightColorOff = glm::vec3(0.5f, 0.2f, 0.2f);
	
	// -----------------------------

	// [POINTEURS INTELLIGENTS POUR LA GESTION DES ENTR�ES ET CAMERA]
	// Gestion des manettes (en dehors de la classe)
	std::shared_ptr<Gamepad> gamepad = nullptr;
	// Gestion du clavier
	std::shared_ptr<Keyboard> keyboard = nullptr;
	// Gestion de la cam�ra
	std::shared_ptr<Camera> camera = nullptr;

	// -----------------------------
	
	// [ATTRIBUTS PHYSIQUE]
	glm::vec3 position;
	glm::vec3 direction;

	// Vitesse
	float currentSpeed = 0.0f;
	float maxSpeed = 22.0f;
	float acceleration = 10.0f;
	float deceleration = 5.0f;

	// Angle de braquage (en degr�s), angle de braquage max. et vitesse de braquage des roues avants
	float steeringAngle = 0.0f;
	float maxSteeringAngle = 30.0f;
	float steeringSpeed = 65.0f;

	// Rotation des roues
	float wheelRotationAngle = 0.0f;
	const float wheelRadius = 0.5f;

	// -----------------------------

	// [PARAMETRES DE REBOND]
	float bounceFactor = 0.5f;         // Amortissement du rebond (0.5 = rebond amorti, 1.0 = rebond parfait)
	const float minBounceSpeed = 0.5f;       // Vitesse minimale pour stopper le rebond
	const float bouncePenetration = 0.7f;    // Rentr�e dans l'obstacle avant rebond

	// Flag et normale pour d�coincer la voiture
	bool isStuck = false;
	glm::vec3 stuckNormal = glm::vec3(0.0f);

	// ------------------------------

	// [PARAMETRES DE TRAIN�E DES ROUES]
	// Utilisation d'une deque pour rapidit� d'ajout/suppression
	std::deque<TrailPoint> trailPointsFrontLeft;
	std::deque<TrailPoint> trailPointsFrontRight;
	std::deque<TrailPoint> trailPointsRearLeft;
	std::deque<TrailPoint> trailPointsRearRight;
	// Couleur de la train�e
	glm::vec3 trailColor = glm::vec3(0.4f, 0.3f, 0.2f);
	// Dur�e de vie d'un point (s) et �paisseur de la tra�n�e
	float trailLifetime = 2.0f;
	float trailThickness = 0.5f;
	// Intervalle minimum entre deux points (pour �viter trop de points) + timer pour dessiner dans l'intervalle
	float trailTimeBetweenTwoPoints = 0.03f;
	float trailTimer = 0.0f;
	// Flag pour activer/d�sactiver les train�es
	bool wheelsTrailsEnabled = true;

	// -----------------------------

	// [PARAMETRES DE TRANSFERT DE POIDS]
	// Transfert de poids en WIP donc a activ� manuellement
	bool weightTransferEnabled = false;
	float weightTransferRoll = 0.0f;
	float weightTransferPitch = 0.0f;

	// Dessine une train�e
	void drawWheelTrail(Shader& trailShader, const glm::mat4& view, const glm::mat4& projection, const std::deque<TrailPoint>& points, float thickness) const;
};

#endif // CAR_H