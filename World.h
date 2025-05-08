#pragma once

#include <vector>
#include <memory>

#include "Car.h"
#include "Frustum.h"
#include "JoystickManager.h"
#include "Terrain.h"
#include "Skybox.h"

class World {
public:
    World(int windowWidth, int windowHeight, GLFWwindow* window);

	// Charge les modèles de voitures
    void loadModels();
    // Recalcul la zone de rendu pour chaque joueur (gestion split screen)
    void calculateViewport(int playerIndex, int& x, int& y, int& w, int& h);

    // Mise à jour de la scène
    void updateScene(float deltaTime);
    // Rendu de la scène
    void renderScene(Shader& shader, Shader& wireframeShader, Shader& terrainShader);

private:
    int width, height;
    GLFWwindow* window;

    Terrain terrain;
    Skybox skybox;
    std::vector<Car> voitures;

    Frustum frustum;

	// Un seul clavier et une seule camera noclip
    std::shared_ptr<Keyboard> keyboard;
    std::shared_ptr<Camera> noClip = nullptr;

	int maxPlayers = 4;

    // -------------------------------

    // Ajoute une voiture au monde à une position donnée
    void addPlayer(const std::string& modelPath, const glm::vec3& startPosition);
	// Dessine les éléments du monde
    void Draw(Shader& shader, Shader& wireframeShader, Shader& terrainShader, int viewportWidth, int viewportHeight, const glm::mat4& view, const glm::vec3& viewPos);
};