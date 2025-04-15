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

	// Model Matrix du body a utilis� dans plusieurs m�thodes (Draw, calcul de bounding box...)
	glm::mat4 getBodyModelMatrix() const;

	// V�rifie si la voiture est visible dans le frustum
	bool isVisible(const Frustum& frustum) const;

	// Dessine la voiture
	void Draw(Shader& shader);

	// -----------------------

	// G�re la physique de la voiture avec des contr�les
	void updatePhysics(float deltaTime, GLFWwindow* window);
private:
	// S�paration des parts du v�hicule (pour futur animation des roues)
	Model body;
	Model wheels[4];

	// Attributs pour la physique
	glm::vec3 position;
	glm::vec3 direction;
};

#endif CAR_H