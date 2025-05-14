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
    void renderScene(Shader& shader, Shader& phareShader, Shader& wireframeShader, Shader& terrainShader, Shader& defaultShader);

private:
    int width, height;
    GLFWwindow* window;

    Terrain terrain;
    Skybox skybox;
    std::vector<Car> voitures;
    Model cactus;
    Model bois_abandon;
    Model pierre1, pierre2, pierre3;
    Model bois1, bois2, bois3;
    Model herbe1, herbe2, herbe3;

    // Liste des matrices modèle des cactus, pierres
    std::vector<glm::mat4> cactusModelMatrices; 
    std::vector<glm::mat4> pierreModelMatrices;
    std::vector<glm::mat4> boisModelMatrices;
    std::vector<glm::mat4> herbeModelMatrices;

	// Lumière du soleil
    glm::vec3 lightDir = glm::normalize(glm::vec3(-0.5f, -1.0f, -0.5f)); // Direction du soleil
    glm::vec3 lightColor = glm::vec3(1.0f, 0.9f, 0.8f);                 // Couleur du soleil

    // Matériau Voiture
    glm::vec3 carAmbientColor = glm::vec3(0.9f, 0.8f, 0.6f);
    glm::vec3 carDiffuseColor = glm::vec3(0.8f, 0.6f, 0.6f);
    // Matériau Cactus
    glm::vec3 cactusAmbientColor = glm::vec3(0.7f, 0.6f, 0.4f);
    glm::vec3 cactusDiffuseColor = glm::vec3(0.4f, 0.7f, 0.4f);
    // Matériau Pierre
    glm::vec3 pierreAmbientColor = glm::vec3(0.6f, 0.4f, 0.4f);
    glm::vec3 pierreDiffuseColor = glm::vec3(0.8f, 0.6f, 0.6f);
	// Matériau Bois
	glm::vec3 boisAmbientColor = glm::vec3(0.5f, 0.4f, 0.3f);
	glm::vec3 boisDiffuseColor = glm::vec3(0.7f, 0.5f, 0.4f);
	// Matériau Herbe
    glm::vec3 herbeAmbientColor = glm::vec3(0.7f, 0.35f, 0.12f);
    glm::vec3 herbeDiffuseColor = glm::vec3(0.85f, 0.45f, 0.18f);

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
    void Draw(Shader& shader, Shader& phareShader, Shader& wireframeShader, Shader& terrainShader, Shader& defaultShader, int viewportWidth, int viewportHeight, const glm::mat4& view, const glm::vec3& viewPos);
	// Génère des modèles aléatoires par rapport au terrain
    void generateModels(std::vector<glm::mat4>& modelMatrices, int count, float minScale, float maxScale);
	// Rendu des modèles
	void renderModels(Shader& shader, const Frustum& frustum, const std::vector<Model*>& models, const std::vector<glm::mat4>& modelMatrices, const glm::vec3& ambientColor, const glm::vec3& diffuseColor);
	// Met à jour la direction de la lumière en fonction du temps
    void updateLightDirection(float deltaTime);
};