#include "Camera.h"
#include <glm/gtx/string_cast.hpp>

Camera::Camera(int width, int height, glm::vec3 position)
{
	Camera::width = width;
	Camera::height = height;
	Position = position;
}

glm::mat4 Camera::getViewMatrix()
{
	// Fais regarder la caméra dans la bonne directon depuis la bonne position
	return glm::lookAt(Position, Position + Orientation, Up);
}

void Camera::Inputs(GLFWwindow* window)
{
	static bool keyPressed = false;

	// [TOUCHES "universelles"]
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
	{
		if (!keyPressed)
		{
			if (mode == NO_CLIP) 
			{ 
				setMode(THIRD_PERSON);
				justSwitchedToThirdPerson = true;
			}
			else 
				setMode(NO_CLIP);

			keyPressed = true;
			std::cout << "Change Camera mode: " << ((mode == NO_CLIP) ? "NO_CLIP" : "THIRD_PERSON") << std::endl;
		}
	}
	else if (glfwGetKey(window, GLFW_KEY_V) == GLFW_RELEASE)
	{
		keyPressed = false;
	}

	// [NO CLIP]
	if (mode == NO_CLIP) {
		// Gestion clavier
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			Position += speed * Orientation;
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			Position += speed * -glm::normalize(glm::cross(Orientation, Up));
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			Position += speed * -Orientation;
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			Position += speed * glm::normalize(glm::cross(Orientation, Up));
		}
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		{
			Position += speed * Up;
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		{
			Position += speed * -Up;
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		{
			speed = 0.05f;
		}
		else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
		{
			speed = 0.01f;
		}

		// Gestion souris
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

			// Évite que la caméra ne saute au 1er clic
			if (firstClick)
			{
				glfwSetCursorPos(window, (width / 2), (height / 2));
				firstClick = false;
			}

			// Récupére les coordoonées de la souris
			double mouseX;
			double mouseY;
			glfwGetCursorPos(window, &mouseX, &mouseY);

			// Normalise et décalle les coord. de la souris pour qu'elles soient au milieu de l'écran entre -1 et 1 & détermine la rotation à partir de ça
			float rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
			float rotY = sensitivity * (float)(mouseX - (width / 2)) / width;

			// Calcule le changement vertical à venir de l'Orientation 
			glm::vec3 newOrientation = glm::rotate(Orientation, glm::radians(-rotX), glm::normalize(glm::cross(Orientation, Up)));
			// Décide si la prochaine orientation verticale est possible ou non (ici, on évite les barrel rolls)
			if (!((glm::angle(newOrientation, Up) <= glm::radians(5.0f)) or (glm::angle(newOrientation, -Up) <= glm::radians(5.0f))))
			{
				Orientation = newOrientation;
			}

			// Rotation de l'Orientation sur l'axe Y (horizontal/gauche-droite)
			Orientation = glm::rotate(Orientation, glm::radians(-rotY), Up);

			// Définit le curseur de la souris au milieu de l'écran pour éviter qu'il ne sorte de la fenêtre
			glfwSetCursorPos(window, (width / 2), (height / 2));
		}
		else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
		{
			// Révèle le curseur de la souris quand on ne regarde plus autour
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			// Réinit. le booléen pour que la caméra ne saute pas au prochain clic
			firstClick = true;
		}
	}

	// [3RD PERSON]
	else 
	{   
		float rotX = 0.0f;
		float rotY = 0.0f;

		// Gestion Manette
		if (_gamepad) {
			float rightStickX = _gamepad->getRightStickX();
			float rightStickY = _gamepad->getRightStickY();

			rotX = rightStickX * gamepadSensitivity;
			rotY = rightStickY * gamepadSensitivity;
		}
		// Gestion Souris
		else
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

			// Récupére les coordoonées de la souris
			double mouseX;
			double mouseY;
			glfwGetCursorPos(window, &mouseX, &mouseY);

			// Normalise et décalle les coord. de la souris pour qu'elles soient au milieu de l'écran entre -1 et 1 & détermine la rotation à partie de ça
			rotX = sensitivity * (float)(mouseX - (width / 2)) / width;
			rotY = sensitivity * (float)(mouseY - (height / 2)) / height;

			// Définit le curseur de la souris au milieu de l'écran pour éviter qu'il ne sorte de la fenêtre
			glfwSetCursorPos(window, (width / 2), (height / 2));
		}

		// Applique/ajuste les rotations (yaw/pitch)
		if (rotX != 0.0f || rotY != 0.0f)
		{
			yaw += rotX;
			pitch += rotY;

			// Limite le pitch pour éviter les inversions (barrel-roll)
			if (pitch > 45.0f) pitch = 45.0f;// vers sol
			if (pitch < -10.0f) pitch = -10.0f;// vers ciel
		}
	}
}

void Camera::followTarget(const glm::vec3& targetPosition, const glm::vec3& targetDirection, const Terrain& terrain)
{
	if (mode == THIRD_PERSON)
	{
		// Quand de retour en mode 3eme personne, on recentre bien derrière la voiture
		if (justSwitchedToThirdPerson)
		{
			// Positionner la caméra directement derrière la voiture
			glm::vec3 offset = -glm::normalize(targetDirection) * distanceToTarget;
			offset.y = 2.0f; // Ajuste la hauteur en fonction de la voiture
			Position = targetPosition + offset;

			// Orienter la caméra vers la voiture (avec notre offset en y va donc regarder légèrement vers le bas)
			Orientation = glm::normalize(targetPosition - Position);

			// Ajuster les angles de rotation pour correspondre à la direction de la voiture
			yaw = glm::degrees(atan2(-targetDirection.z, targetDirection.x));
			pitch = glm::degrees(asin(targetDirection.y)) + 15.0f; // offset pour voir la voiture d'un peu plus haut

			// Change le flag pour les prochains changements de caméra
			justSwitchedToThirdPerson = false;
		}
		else
		{
			// Calcule position cam par rapport aux angles de rotation (conversion coord. spheriques en cartesienne)
			glm::vec3 offset;
			offset.x = distanceToTarget * (cos(glm::radians(pitch))) * cos(glm::radians(yaw));
			offset.y = distanceToTarget * sin(glm::radians(pitch)); // Ajuste la hauteur en fonction du terrain;
			offset.z = distanceToTarget * (cos(glm::radians(pitch))) * sin(glm::radians(yaw));

			// Positionne la caméra autour de la voiture
			Position = targetPosition + offset;

			// Ajuste la position de la caméra par rapport au terrain
			float terrainHeight = terrain.getHeightAt(Position.x, Position.z);
			if (Position.y < terrainHeight + 2.0f) // offset de 2 pour être un peu au dessus du terrain
			{
				Position.y = terrainHeight + 2.0f;
			}

			// Oriente la caméra vers la voiture
			Orientation = glm::normalize(targetPosition - Position);
		}
	}
}

void Camera::Update(const glm::vec3& targetPosition, const glm::vec3& targetDirection, const Terrain& terrain)
{
	// Si la caméra est en mode troisième personne, suit la voiture
	if (mode == THIRD_PERSON) followTarget(targetPosition, targetDirection, terrain);
}

