#ifndef WORLD_H
#define WORLD_H

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

    // Charge les mod�les de voitures
    void loadModels();
    // Mise � jour de la sc�ne
    void updateScene(float deltaTime);
    // Rendu de la sc�ne
    void renderScene(Shader& defaultShader, Shader& terrainShader, Shader& carShader, Shader& headlightShader, Shader& wireframeShader);

private:
    int width, height;
    GLFWwindow* window;

    Frustum frustum;

    // Un seul clavier et une seule camera noclip
    std::shared_ptr<Keyboard> keyboard;
    std::shared_ptr<Camera> noClip = nullptr;

    // Flag pour activer/d�sactiver les wireframes
    bool isWireframeEnable = false;

    // -------------------------------

    // Mod�les 3D
    Terrain terrain;
    Skybox skybox;
    std::vector<Car> cars;
    Model cactus1, cactus2, cactus3;
    Model rock1, rock2, rock3;
    Model wood1, wood2, wood3;
    Model grass1, grass2, grass3;
    Model palmTree;
    Model buffaloSkull;
    Model cart;

    // Liste des matrices mod�le des cactus, pierres, ...
    std::vector<glm::mat4> cactiModelMatrices; 
    std::vector<glm::mat4> rocksModelMatrices;
    std::vector<glm::mat4> woodsModelMatrices;
    std::vector<glm::mat4> grassModelMatrices;
	std::vector<glm::mat4> bonesModelMatrices;
    glm::mat4 cartModelMatrice;

	// Propri�t�s de la lumi�re (soleil)
    glm::vec3 lightDir = glm::normalize(glm::vec3(-0.5f, -1.0f, -0.5f));
    glm::vec3 lightColor = glm::vec3(1.0f, 0.9f, 0.8f);

    // Mat�riau Voiture
    glm::vec3 carAmbientColor = glm::vec3(0.9f, 0.8f, 0.6f);
    glm::vec3 carDiffuseColor = glm::vec3(0.8f, 0.6f, 0.6f);
    // Mat�riau Cactus
    glm::vec3 cactiAmbientColor = glm::vec3(0.6f, 0.5f, 0.4f);
    glm::vec3 cactiDiffuseColor = glm::vec3(0.5f, 0.6f, 0.5f);
    // Mat�riau Pierre
    glm::vec3 rocksAmbientColor = glm::vec3(0.6f, 0.4f, 0.4f);
    glm::vec3 rocksDiffuseColor = glm::vec3(0.8f, 0.6f, 0.6f);
	// Mat�riau Bois
	glm::vec3 woodsAmbientColor = glm::vec3(0.5f, 0.4f, 0.3f);
	glm::vec3 woodsDiffuseColor = glm::vec3(0.7f, 0.5f, 0.4f);
	// Mat�riau Herbe
    glm::vec3 grassAmbientColor = glm::vec3(0.7f, 0.35f, 0.12f);
    glm::vec3 grassDiffuseColor = glm::vec3(0.85f, 0.45f, 0.18f);
    // Mat�riau Os
	glm::vec3 bonesAmbientColor = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::vec3 bonesDiffuseColor = glm::vec3(0.7f, 0.7f, 0.7f);

    // -------------------------------

	// Bounding Box des objets statiques pour gestion des collisions
    std::vector<BoundingBox> staticObstacles;

    // -------------------------------

	// Temps �coul� depuis le d�but du cycle jour-nuit
    float dayNightCycleTime = 0.0f;
    // Dur�e d'un cycle complet
    float dayNightCycleDuration = 90.0f;
	// Transition fluide jour-nuit
    float dayNightBlend = 0.0f;
    // Intensit� de la lumi�re
    float lightIntensity = 1.0f;

    // -------------------------------

    // Ajoute une voiture au monde � une position donn�e
    void addCar(const std::string& modelPath, const glm::vec3& startPosition);
    // Ajouter ou retirer des joueurs
    void addRemovePlayers();
	// Bind les contr�leurs (manettes/clavier)
    void bindController();

    // Recalcul la zone de rendu pour chaque joueur (gestion split screen)
    void calculateViewport(int playerIndex, int& x, int& y, int& w, int& h);

	// V�rifie si une voiture est en mode no clip
    bool isAnyCarInNoClip() const;

	// Dessine les �l�ments du monde
    void Draw(Shader& defaultShader, Shader& terrainShader, Shader& carShader, Shader& headlightShader, Shader& wireframeShader, int viewportWidth, int viewportHeight, const glm::mat4& view);
	// Dessine les bounding box de tous les mod�les en wireframe
    void drawAllModelCollisionBoxes(Shader& wireframeShader, const glm::mat4& view, const glm::mat4& projection);
	// G�n�re des mod�les al�atoires par rapport au terrain
    void generateModels(std::vector<glm::mat4>& modelMatrices, int count, float minScale, float maxScale);
	// Rendu des mod�les
	void renderModels(Shader& shader, const std::vector<Model*>& models, const std::vector<glm::mat4>& modelMatrices, const glm::vec3& ambientColor, const glm::vec3& diffuseColor);
	
    // Met � jour la direction de la lumi�re en fonction du temps
    void updateLightDirection(float deltaTime);
	// Met � jour le cycle jour-nuit
    void updateDayNightCycle(float deltaTime);

	// Ajoute les bounding box transform�es des mod�les 3D dans un vecteur pour la gestion des collisions
    void addStaticObstacles(const std::vector<Model*>& models, const std::vector<glm::mat4>& modelMatrices);
};

#endif // WORLD_H