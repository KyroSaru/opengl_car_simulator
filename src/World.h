#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <memory>

#include "Car.h"
#include "Frustum.h"
#include "JoystickManager.h"
#include "Terrain.h"
#include "Skybox.h"


// Pour éviter une inclusion circulaire
class WorldEditorUI;

// [STRUCTURES POUR LISIBILITE] (pour changer ces variables dynamiquement depuis notre UI)
// Stocke la position de départ, rotation et hométhétie des objets 
struct modelTransfos
{
    glm::vec3 position;
    float rotation;
    float scale;
    glm::mat4 modelMatrix;
};

// Gére les paramètres de génération des modèles
struct generationParams {
    int count;
    float minScale;
    float maxScale;
};

class World 
{
public:
    World(int windowWidth, int windowHeight, GLFWwindow* window);

    // Getters/Setters
    void setEditorUI(WorldEditorUI* ui) { worldEditorUI = ui; }
	Terrain& getTerrain() { return terrain; }
	float getTerrainHeight() const { return terrainHeight; }
	void setTerrainHeight(float h) { terrainHeight = h; }
    float getGlobalScale() const { return globalScale; }
    void setGlobalScale(float s) { globalScale = s; }
    std::vector<Car>& getCars() { return cars; }
	
    // Getters des matrices de transfos
	std::vector<modelTransfos>& getCactiModelMatrices() { return cactiModelMatrices; }
	std::vector<modelTransfos>& getRocksModelMatrices() { return rocksModelMatrices; }
	std::vector<modelTransfos>& getWoodsModelMatrices() { return woodsModelMatrices; }
	std::vector<modelTransfos>& getGrassModelMatrices() { return grassModelMatrices; }
	std::vector<modelTransfos>& getBonesModelMatrices() { return bonesModelMatrices; }
	// Getters des matériaux
    materialColors& getTerrainColors() { return terrainColors; }
    materialColors& getCactiColors() { return cactiColors; }
    materialColors& getRocksColors() { return rocksColors; }
    materialColors& getWoodsColors() { return woodsColors; }
    materialColors& getGrassColors() { return grassColors; }
    materialColors& getBonesColors() { return bonesColors; }
	// Getters/Setters des paramètres de génération
	generationParams& getCactiParams() { return cactiParams; }
	generationParams& getRocksParams() { return rocksParams; }
	generationParams& getWoodsParams() { return woodsParams; }
	generationParams& getGrassParams() { return grassParams; }
	generationParams& getBonesParams() { return bonesParams; }
	// Getters/Setters du cycle
    void setDayNightCycleDuration(float duration) { dayNightCycleDuration = duration; }
    float getDayNightCycleDuration() const { return dayNightCycleDuration; }
    void setDayNightCycleHour(float hour);
    float getDayNightCycleHour() const;
    void setLockProgress(bool lock) { lockProgress = lock; }
    bool getLockProgress() const { return lockProgress; }

    // Charge les modèles de voitures
    void loadModels();
    // Mise à jour de la scène
    void updateScene(float deltaTime);
    // Rendu de la scène
    void renderScene(Shader& defaultShader, Shader& terrainShader, Shader& carShader, Shader& headlightShader, Shader& wireframeShader, Shader& trailShader);

	// Gestion de l'affichage de l'UI
    void toggleEditorUI();
    // Génère (de zéro) des modèles aléatoires par rapport au terrain 
    void generateModels(std::vector<modelTransfos>& modelMatrices, int count, float minScale, float maxScale);
	// Regénère les modèles sur le terrain (en gardant les anciennes informations en compte)
	void regenerateModels(std::vector<modelTransfos>& modelMatrices);
private:
	// [GLOBALES]
    const int width, height;
    GLFWwindow* window;

	Frustum frustum;

	WorldEditorUI* worldEditorUI = nullptr;

    // Un seul clavier et une seule camera noclip
    std::shared_ptr<Keyboard> keyboard;
    std::shared_ptr<Camera> noClip = nullptr;

    // Flag pour activer/désactiver les wireframes
    bool isWireframeEnable = false;
	// Flag pour activer/désactiver l'UI de l'éditeur
	bool isUIEnable = false;

    // -------------------------------

    // [MODELES 3D, LUMIERE & COULEURS] 
    // Modèles 3D
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

    // Liste des matrices modèle des cactus, pierres, ...
    std::vector<modelTransfos> cactiModelMatrices;
    std::vector<modelTransfos> rocksModelMatrices;
    std::vector<modelTransfos> woodsModelMatrices;
    std::vector<modelTransfos> grassModelMatrices;
	std::vector<modelTransfos> bonesModelMatrices;
    //glm::mat4 cartModelMatrice = glm::mat4(1.0f);

	// Propriétés de la lumière (soleil)
    glm::vec3 lightDir = glm::normalize(glm::vec3(-0.5f, -1.0f, -0.5f));
    glm::vec3 lightColor = glm::vec3(1.0f, 0.9f, 0.8f);

    // Matériaux
    materialColors terrainColors = { glm::vec3(0.8f, 0.5f, 0.3f), glm::vec3(0.7f, 0.5f, 0.4f) };
    materialColors cactiColors   = { glm::vec3(0.4f, 0.35f, 0.3f), glm::vec3(0.5f, 0.6f, 0.5f) };
    materialColors rocksColors   = { glm::vec3(0.6f, 0.4f, 0.4f), glm::vec3(0.8f, 0.6f, 0.6f) };
    materialColors woodsColors   = { glm::vec3(0.5f, 0.4f, 0.3f), glm::vec3(0.7f, 0.5f, 0.4f) };
    materialColors grassColors   = { glm::vec3(0.5f, 0.4f, 0.3f), glm::vec3(0.4f, 0.5f, 0.4f) };
    materialColors bonesColors   = { glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.7f, 0.7f, 0.7f) };

    // -------------------------------

    // [PARAMETRE GENERATION]
	float terrainHeight = 45.0f;

    float globalScale = 1.0f;

	generationParams cactiParams = { 50, 2.0f, 3.5f };
	generationParams rocksParams = { 60, 1.0f, 2.5f };
	generationParams woodsParams = { 25, 2.0f, 3.5f };
	generationParams grassParams = { 25, 2.0f, 3.5f };
	generationParams bonesParams = { 25, 2.0f, 3.5f };

    // -------------------------------

	// Bounding Box des objets statiques pour gestion des collisions
    std::vector<BoundingBox> staticObstaclesCollisionBoxes;

    // -------------------------------

    // [CYCLE JOUR-NUIT]
	// Temps écoulé depuis le début du cycle jour-nuit
    float dayNightCycleTime;
    // Durée d'un cycle complet (en secondes)
    float dayNightCycleDuration = 90.0f;
    // Bloquer la progression du cycle jour-nuit
    bool lockProgress = false;
	// Transition fluide jour-nuit
    float dayNightBlend = 0.0f;
    // Intensité de la lumière
    float lightIntensity = 1.0f;

    // -------------------------------

    // Ajoute une voiture au monde à une position donnée
    void addCar(const std::string& modelPath, const glm::vec3& startPosition);
    // Ajouter ou retirer des joueurs
    void addRemovePlayers();
	// Bind les contrôleurs (manettes/clavier)
    void bindController();

    // Recalcul la zone de rendu pour chaque joueur (gestion split screen)
    void calculateViewport(int playerIndex, int& x, int& y, int& w, int& h);

	// Vérifie si une voiture est en mode no clip
    bool isAnyCarInNoClip() const;

	// Dessine les éléments du monde
    void Draw(Shader& defaultShader, Shader& terrainShader, Shader& carShader, Shader& headlightShader, Shader& wireframeShader, Shader& trailShader, int viewportWidth, int viewportHeight, const glm::mat4& view);
	// Dessine les bounding box de tous les modèles en wireframe
    void drawAllModelCollisionBoxes(Shader& wireframeShader, const glm::mat4& view, const glm::mat4& projection);
	// Rendu des modèles
	void renderModels(Shader& shader, const std::vector<Model*>& models, const std::vector<modelTransfos>& modelMatrices, const materialColors& colors);
	
    // Met à jour la direction de la lumière en fonction du temps
    void updateLightDirection(float deltaTime);
	// Met à jour le cycle jour-nuit
    void updateDayNightCycle(float deltaTime);

	// Ajoute les bounding box transformées des modèles 3D dans un vecteur pour la gestion des collisions
    void addStaticObstacles(const std::vector<Model*>& models, const std::vector<modelTransfos>& modelMatrices);

	// Vérifie si l'état du flag isUIEnable
    bool isEditorUIVisible() const { return isUIEnable; }
};

#endif // WORLD_H