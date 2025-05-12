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
    void renderScene(Shader& shader, Shader& wireframeShader, Shader& terrainShader, Shader& cactusShader);

private:
    int width, height;
    GLFWwindow* window;

    Terrain terrain;
    Skybox skybox;
    std::vector<Car> voitures;
    Model cactus;
    Model virevoltant;
    std::vector<glm::mat4> cactusModelMatrices; // Liste des matrices modèle des cactus

	// Lumière du soleil
    glm::vec3 lightDir = glm::normalize(glm::vec3(-0.5f, -1.0f, -0.5f)); // Direction du soleil
    glm::vec3 lightColor = glm::vec3(1.0f, 0.9f, 0.8f);                 // Couleur du soleil

    // Matériau Cactus
    glm::vec3 cactusAmbientColor = glm::vec3(0.7f, 0.6f, 0.4f);
    glm::vec3 cactusDiffuseColor = glm::vec3(0.4f, 0.7f, 0.4f);

    // Matériau Voiture
    glm::vec3 carAmbientColor = glm::vec3(0.9f, 0.8f, 0.6f);
    glm::vec3 carDiffuseColor = glm::vec3(0.8f, 0.6f, 0.6f);

    Frustum frustum;

	// Un seul clavier et une seule camera noclip
    std::shared_ptr<Keyboard> keyboard;
    std::shared_ptr<Camera> noClip = nullptr;

	int maxPlayers = 4;

    // -------------------------------

    // Ajoute une voiture au monde à une position donnée
    void addPlayer(const std::string& modelPath, const glm::vec3& startPosition);
    void worldInput();
    void bindController();
	// Dessine les éléments du monde
    void Draw(Shader& shader, Shader& wireframeShader, Shader& terrainShader, Shader& cactusShader, int viewportWidth, int viewportHeight, const glm::mat4& view, const glm::vec3& viewPos);
	// Génère des cactus aléatoires
    void generateCacti(int count);
	// Met à jour la direction de la lumière en fonction du temps
    void updateLightDirection(float deltaTime);
};