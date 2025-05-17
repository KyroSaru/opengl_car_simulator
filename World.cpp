#include "World.h"

#include <random>

World::World(int windowWidth, int windowHeight, GLFWwindow* win)
    : width(windowWidth), height(windowHeight), window(win),
    keyboard(std::make_shared<Keyboard>()), 
    terrain("models/terrain/heightmap.png", 35.0f), 
    cart("models/chariot/chariot.gltf"),
    cactus1("models/cactus/cactus1.gltf"), cactus2("models/cactus/cactus2.gltf"), cactus3("models/cactus/cactus3.gltf"),
    rock1("models/pierre/pierre1.gltf"), rock2("models/pierre/pierre2.gltf"), rock3("models/pierre/pierre3.gltf"),
    wood1("models/bois/bois1.gltf"), wood2("models/bois/bois2.gltf"), wood3("models/bois/bois3.gltf"),
    grass1("models/herbe/herbe1.gltf"), grass2("models/herbe/herbe2.gltf"), grass3("models/herbe/herbe3.gltf"),
    palmTree("models/palmier/palmier.gltf"),
    buffaloSkull("models/crane/crane.gltf")
{
    JoystickManager::init();

	// --- [AJUSTEMENT BOUNDING BOX] ---
    // Cactus
	cactus1.collisionBox.adjustMin(0.6f, 0.0f, 0.6f);
	cactus1.collisionBox.adjustMax(0.6f, 0.0f, 0.6f);
	cactus2.collisionBox.adjustMin(0.5f, 0.0f, 0.2f);
	cactus2.collisionBox.adjustMax(0.5f, 0.0f, 0.2f);
	cactus3.collisionBox.adjustMin(0.15f, 0.0f, 0.15f);
	cactus3.collisionBox.adjustMax(0.15f, 0.0f, 0.15f);

    // Pierres
    rock1.collisionBox.adjustMin(0.2f, 0.0f, 0.2f);
	rock1.collisionBox.adjustMax(0.35f, 0.1f, 0.3f);
	rock2.collisionBox.adjustMin(0.4f, 0.0f, 0.25f);
	rock2.collisionBox.adjustMax(0.35f, 0.0f, 0.2f);
	rock3.collisionBox.adjustMin(0.25f, 0.0f, 0.25f);
	rock3.collisionBox.adjustMax(0.25f, 0.15f, 0.25f);
	
    // Bois
	wood1.collisionBox.adjustMin(0.25f, 0.0f, 0.6f);
	wood1.collisionBox.adjustMax(0.15f, 0.0f, 0.4f);
	wood2.collisionBox.adjustMin(0.4f, 0.0f, 0.5f);
	wood2.collisionBox.adjustMax(0.1f, 0.0f, 0.5f);
	wood3.collisionBox.adjustMin(0.15f, 0.0f, 0.35f);
	wood3.collisionBox.adjustMax(0.4f, 0.0f, 0.35f);
	palmTree.collisionBox.adjustMin(2.0f, 0.0f, 2.0f);
	palmTree.collisionBox.adjustMax(2.3f, 0.0f, 1.9f);

    // Os
	buffaloSkull.collisionBox.adjustMin(0.2f, 0.0f, 0.8f);
	buffaloSkull.collisionBox.adjustMax(0.2f, 0.0f, 0.8f);

    // Chariot
	cart.collisionBox.adjustMin(0.0f, 0.0f, 0.2f);
	cart.collisionBox.adjustMax(0.0f, 0.0f, 0.2f);
}

// ---------------------------------------------


void World::loadModels()
{
    addCar("models/voiture_gltf/voiture1.gltf", glm::vec3(0.0f, 2.0f, 0.0f));

	// Génères aléatoirement des modèles de cactus, pierres, bois, ...
    generateModels(cactiModelMatrices, 50, 1.5f, 3.0f);
    generateModels(rocksModelMatrices, 60, 0.5f, 2.5f);
    generateModels(woodsModelMatrices, 25, 1.5f, 2.5f);
    generateModels(grassModelMatrices, 40, 0.5f, 1.5f);
	generateModels(bonesModelMatrices, 20, 0.5f, 1.5f);
}

void World::addCar(const std::string& modelPath, const glm::vec3& startPosition) 
{

    if (cars.size() >= 4)
    {
		std::cout << "Maximum number of players reached! [4]\n" << std::endl;
        return;
    }

	// Création de la voiture & positionnement
    Car car(modelPath);
    car.setPosition(startPosition);

	// Création de la caméra 3rd Person pour la voiture
    auto camera = std::make_shared<Camera>(width, height, startPosition);
    camera->setMode(THIRD_PERSON);
    car.setCamera(camera);

	// Ajout de la voiture à la liste
    cars.push_back(car);
}

// ---------------------------------------------

void World::addRemovePlayers() 
{
    if (keyboard->wasEnterPressed(window))
        addCar("models/voiture_gltf/voiture" + std::to_string(cars.size() + 1) + ".gltf", cars[cars.size() - 1].getPosition() + glm::vec3(15.0f, 0.0f, 0.0f)); // pas optimal rien ne nous dit que cette position est libre ...

    if (keyboard->wasBackspacePressed(window)) {
        // Si plus qu'un joueur, on ne retire par le dernier joueur
        if (cars.size() > 1) 
        {
            auto& car = cars.back();

			std::cout << "Player " << car.getId() + 1 << " removed from the game...\n";

			// Si la voiture a une manette, on la désassigne
            if (car.hasGamepad())
                car.getGamepad()->Unassign();

            // Si la voiture avait le clavier, on le désassigne aussi
            if (car.hasKeyboard())
                keyboard->Unassign();

            cars.pop_back();
        }
    }
}

void World::bindController() 
{
    // Cherche tous les gamepads disponibles
    auto availableGamepads = JoystickManager::findAllAvailableGamepads();

    for (auto& car : cars) {
        // Ni clavier, ni manette
        if (!car.hasGamepad() && !car.hasKeyboard()) { // remplacer pas isAssigned ?
            // Manettes dispo.
            if (!availableGamepads.empty()) {
                auto gamepad = availableGamepads.back();
                availableGamepads.pop_back();

                // Vérifier que la manette n'est pas déjà assigné
                    gamepad->Assign();
                    car.setGamepad(gamepad);
                    car.getCamera()->setGamepad(gamepad);
                    std::cout << "Player " << car.getId() + 1 << " is using gamepad " << gamepad->getGamepadID() << ": " << glfwGetJoystickName(gamepad->getGamepadID()) << std::endl;
            }
            // Clavier disponible
            else if (!keyboard->isAssigned())
            {
                keyboard->Assign();
                car.setKeyboard(keyboard);
                car.getCamera()->setGamepad(nullptr);
				std::cout << "Player " << car.getId() + 1 << " is using keyboard.\n";
            }
        }
        // Manette déconnectée
        else if (car.hasGamepad() && !JoystickManager::isConnected(car.getGamepad()->getGamepadID())) {
            auto currentGamepad = car.getGamepad();
            currentGamepad->Unassign();
            car.setGamepad(nullptr);

            // Si le clavier n'est pas assigné à une autre voiture, on peut l'assigner
			if (!keyboard->isAssigned()) {
				keyboard->Assign();
				car.setKeyboard(keyboard);
                car.getCamera()->setGamepad(nullptr);
			}
			else {
				std::cout << "Keyboard already assigned to another player.\n";
			}

			std::cout << "Gamepad " << currentGamepad->getGamepadID() + 1 << " disconnected! Switching to keyboard.\n";
        }
    }
}

void World::calculateViewport(int playerIndex, int& x, int& y, int& w, int& h) 
{
    switch (cars.size())
    {
        case 1:
            // Plein écran
            x = 0;
            y = 0;
            w = width;
            h = height;
        break;
        case 2:
            // Split vertical
            w = width / 2;
            h = height;
            x = (playerIndex == 0) ? 0 : w;
            y = 0;
        break;
        case 3:
            // Split vertical (partie haute) & horizontal (partie basse)
            if (playerIndex == 0)
            {
                // Haut Gauche
                w = width / 2;
                h = height / 2;
                x = 0;
                y = height / 2;
            }
            else if (playerIndex == 1)
            {
				// Haut Droite
                w = width / 2;
                h = height / 2;
                x = width / 2;
                y = height / 2;
            }
            else
            {
                // Bas
                w = width;
                h = height / 2;
                x = 0;
                y = 0;
            }
          break;
          case 4:
                // Split horizontal & vertical
                w = width / 2;
                h = height / 2;
                x = (playerIndex % 2) * w;
                y = (1 - (playerIndex / 2)) * h; // inverse l'ordre vertical (1er en haut à gauche)
          break;
    }
}

bool World::isAnyCarInNoClip() const
{
    for (const auto& car : cars) {
        auto camera = car.getCamera();

        if (camera && camera->getMode() == NO_CLIP)
            return true;
    }

    return false;
}

// ---------------------------------------------

void World::updateScene(float deltaTime)
{
    // MAJ la direction de la lumière
    updateLightDirection(deltaTime);
    // MAJ le cycle jour-nuit
    updateDayNightCycle(deltaTime);

    // Bind les controlleurs
    bindController();
    // Ajouter ou retirer des joueurs
    addRemovePlayers();

    for (auto& car : cars) {
        // Physique pris en compte qu'en 3rd Person (car no clip utilisé pour le debug)
        if (car.getCamera()->getMode() == THIRD_PERSON && !isAnyCarInNoClip()) {
            car.updatePhysics(deltaTime, window, terrain, cars, staticObstacles);
        }

        // MAJ de la caméra et Gestion des touches
        car.getCamera()->Update(car.getBodyPosition(), car.getDirection(), terrain);
        car.getCamera()->Inputs(window, deltaTime);

        // ------------------------

        // [PHARES]
        // Clavier
        if (car.hasKeyboard() && keyboard->wasHPressed(window)) {
            car.setHeadlightsOn(!car.getHeadlightsOn());
            std::cout << "Headlights " << (car.getHeadlightsOn() ? "ON" : "OFF") << " for player " << car.getId() + 1 << std::endl;
        }
        // Manette
        if (car.hasGamepad() && car.getGamepad()->isYPressed()) {
            car.setHeadlightsOn(!car.getHeadlightsOn());
            std::cout << "Headlights " << (car.getHeadlightsOn() ? "ON" : "OFF") << " for player " << car.getId() + 1 << std::endl;
        }

        // [MODE WIREFRAME]
        // Clavier
        if (car.hasKeyboard() && keyboard->wasWPressed(window))
            isWireframeEnable = !isWireframeEnable;
        // Manette
        if (car.hasGamepad() && car.getGamepad()->isXPressed())
            isWireframeEnable = !isWireframeEnable;
    }	

	// Met à jour le vecteur d'obstacles statiques pour les collisions
	staticObstacles.clear();
	addStaticObstacles({ &cactus1, &cactus2, &cactus3 }, cactiModelMatrices);
    addStaticObstacles({ &rock1, &rock2, &rock3 }, rocksModelMatrices);
    addStaticObstacles({ &wood1, &wood2, &wood3, &palmTree }, woodsModelMatrices);
    addStaticObstacles({ &buffaloSkull }, bonesModelMatrices);
    staticObstacles.push_back(cart.collisionBox.getTransformed(cartModelMatrice));
}

void World::renderScene(Shader& defaultShader, Shader& terrainShader, Shader& carShader, Shader& headlightShader, Shader& wireframeShader)
{
    // Définit la couleur de fond de la fenêtre (RGBA)
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    // Nettoie le back buffer et le depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Vérifie si on passe en No Clip
    for (auto& car : cars) {
        if (car.getCamera()->getMode() == NO_CLIP) {
            noClip = car.getCamera();
            break;
        }
        noClip = nullptr;
    }

    if (noClip) {
        // [NO CLIP]
        glViewport(0, 0, width, height);
        Draw(defaultShader, terrainShader, carShader, headlightShader, wireframeShader, width, height, noClip->getViewMatrix());
    }
    else {
        // [3RD PERSON]
        for (int i = 0; i < cars.size(); ++i) {
            int x, y, w, h;
            calculateViewport(i, x, y, w, h);
            glViewport(x, y, w, h);

            glEnable(GL_SCISSOR_TEST);
            glScissor(x, y, w, h);
            {
                auto camera = cars[i].getCamera();
                Draw(defaultShader, terrainShader, carShader, headlightShader, wireframeShader, w, h, camera->getViewMatrix());
            }
            glDisable(GL_SCISSOR_TEST);
        }
    }
}

void World::Draw(Shader& defaultShader, Shader& terrainShader, Shader& carShader, Shader& headlightShader, Shader& wireframeShader, int viewportWidth, int viewportHeight, const glm::mat4& view)
{
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)viewportWidth / (float)viewportHeight, 0.1f, 200.0f);
	// Calcul du frustum à partir de la matrice de proj. et de vue
    frustum.calculateFrustum(projection, view);

    // --- [TERRAIN] ---
    terrainShader.Activate();
    terrainShader.setMat4("projection", projection);
    terrainShader.setMat4("view", view);
    terrainShader.setFloat("lightIntensity", lightIntensity);
    terrainShader.setVec3("lightDir", lightDir);
    terrainShader.setVec3("lightColor", lightColor);
    
    glm::mat4 terrainModel = glm::mat4(1.0f);
    terrainModel = glm::translate(terrainModel, glm::vec3(0.0f, -20.0f, 0.0f));
    terrain.setModelMatrix(terrainModel);
    terrain.Draw(terrainShader);

    // --- [CIEL] ---    
    skybox.Draw(view, projection, static_cast<float>(glfwGetTime()));

    // --- [MODELES REAGISSANT AU SOLEIL] ---
    defaultShader.Activate();
	defaultShader.setMat4("projection", projection);
	defaultShader.setMat4("view", view);
    defaultShader.setFloat("lightIntensity", lightIntensity);
    defaultShader.setVec3("lightDir", lightDir);
    defaultShader.setVec3("lightColor", lightColor);
    // Cactus
    renderModels(defaultShader, { &cactus1, &cactus2, &cactus3 }, cactiModelMatrices, cactiAmbientColor, cactiDiffuseColor);
    // Pierres
    renderModels(defaultShader, { &rock1, &rock2, &rock3 }, rocksModelMatrices, rocksAmbientColor, rocksDiffuseColor);
    // Bois
    renderModels(defaultShader, { &wood1, &wood2, &wood3, &palmTree }, woodsModelMatrices, woodsAmbientColor, woodsDiffuseColor);
    // Herbes
    renderModels(defaultShader, { &grass1, &grass2, &grass3 }, grassModelMatrices, grassAmbientColor, grassDiffuseColor);
    // Os
	renderModels(defaultShader, { &buffaloSkull }, bonesModelMatrices, bonesAmbientColor, bonesDiffuseColor);

    // -- [CHARIOT] ---
    cartModelMatrice = glm::mat4(1.0f);
    cartModelMatrice = glm::translate(cartModelMatrice, glm::vec3(5.0f, -6.5f, 6.0f));
    cartModelMatrice = glm::rotate(cartModelMatrice, glm::radians(225.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    cartModelMatrice = glm::scale(cartModelMatrice, glm::vec3(1.75f, 1.75f, 1.75f));
    defaultShader.setMat4("model", cartModelMatrice);
    cart.Draw(defaultShader);

	// --- [VOITURE] --- (chargé en dernier pour éviter les erreurs de profondeur avec les vitres)
    carShader.Activate();
    carShader.setMat4("projection", projection);
    carShader.setMat4("view", view);
	carShader.setFloat("lightIntensity", lightIntensity);
    carShader.setVec3("lightDir", lightDir);
    carShader.setVec3("lightColor", lightColor);
    carShader.setVec3("ambientColor", carAmbientColor);
    carShader.setVec3("diffuseColor", carDiffuseColor);

    headlightShader.Activate();
    headlightShader.setMat4("projection", projection);
    headlightShader.setMat4("view", view);

    for (auto& car : cars) {
        if (car.isVisible(frustum)) {
            car.Draw(carShader, headlightShader);
        }
    }

    // --- [WIREFRAME COLLISION BOX] ---
    if (isWireframeEnable) drawAllModelCollisionBoxes(wireframeShader, view, projection);
}

void World::drawAllModelCollisionBoxes(Shader& wireframeShader, const glm::mat4& view, const glm::mat4& projection)
{
    wireframeShader.Activate();
    wireframeShader.setMat4("view", view);
    wireframeShader.setMat4("projection", projection);

    // Fonction lambda pour dessiner les bounding boxes
    auto drawCollisionBoxes = [&](const std::vector<Model*>& models, const std::vector<glm::mat4>& modelMatrices) 
    {
        for (size_t i = 0; i < modelMatrices.size(); ++i) 
        {
            Model* model = models[i % models.size()];
            const glm::mat4& modelMatrix = modelMatrices[i];

            // Visible dans le frustum
            if (model->isVisible(frustum, modelMatrix)) {
                wireframeShader.setMat4("model", modelMatrix);
                model->collisionBox.drawWireframe(wireframeShader);
            }
        }
    };
    drawCollisionBoxes({ &cactus1, &cactus2, &cactus3 }, cactiModelMatrices);
    drawCollisionBoxes({ &rock1, &rock2, &rock3 }, rocksModelMatrices);
    drawCollisionBoxes({ &wood1, &wood2, &wood3, &palmTree }, woodsModelMatrices);
    drawCollisionBoxes({ &grass1, &grass2, &grass3 }, grassModelMatrices);
    drawCollisionBoxes({ &buffaloSkull }, bonesModelMatrices);

    // Chariot
    if (cart.isVisible(frustum, cartModelMatrice)) {
        wireframeShader.setMat4("model", cartModelMatrice);
        cart.collisionBox.drawWireframe(wireframeShader);
    }

    // Voiture
    for (auto& car : cars) {
        if (car.isVisible(frustum)) {
            car.DrawWireframes(wireframeShader, view, projection);
        }
    }
}

// ---------------------------------------------

void World::generateModels(std::vector<glm::mat4>& modelMatrices, int count, float minScale, float maxScale) {
    for (int i = 0; i < count; ++i) {
        // Génère des coord. aléatoires (x, z)
        float x = static_cast<float>(rand() % static_cast<int>(terrain.getWidth() * terrain.getScale())) - terrain.getWidth() * terrain.getScale() / 2.0f;
        float z = static_cast<float>(rand() % static_cast<int>(terrain.getHeight() * terrain.getScale())) - terrain.getHeight() * terrain.getScale() / 2.0f;
        // Récupère la hauteur du terrain à cette position
        float y = terrain.getHeightAt(x, z);

        // Génère homotéthie et rotation aléatoires
        float scale = minScale + static_cast<float>(rand()) / RAND_MAX * (maxScale - minScale);
        float rotation = static_cast<float>(rand() % 360);

        // Matrice Model du modèle (transfos)
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x, y - 20.0f, z));
        model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(scale));

        // Stocke la matrice modèle
        modelMatrices.push_back(model);
    }
}

void World::renderModels(Shader& shader, const std::vector<Model*>& models, const std::vector<glm::mat4>& modelMatrices, const glm::vec3& ambientColor, const glm::vec3& diffuseColor)
{
    shader.Activate();

    // Matériau du modèle
    shader.setVec3("ambientColor", ambientColor);
	shader.setVec3("diffuseColor", diffuseColor);

	for (size_t i = 0; i < modelMatrices.size(); ++i) {
		Model* model = models[i % models.size()];

		// Vérifie si le modèle est visible dans le frustum
		if (model->isVisible(frustum, modelMatrices[i])) {
			shader.setMat4("model", modelMatrices[i]);
			model->Draw(shader);
		}
	}
}

// ---------------------------------------------

void World::updateLightDirection(float deltaTime)
{
	// Applique la rotation à la direction de la lumière
	lightDir = glm::rotate(lightDir, 0.5f * deltaTime, glm::vec3(0.0f, 1.0f, 0.0f));
}

void World::updateDayNightCycle(float deltaTime)
{
    // Avance le temps du cycle jour-nuit
    dayNightCycleTime += deltaTime;
    float progress = fmod(dayNightCycleTime, dayNightCycleDuration) / dayNightCycleDuration;

    // Calcul du blend pour la skybox (transition douce)
    dayNightBlend = 0.5f * (cos(progress * 2.0f * glm::pi<float>()) + 1.0f);

    // Calcul de l’intensité lumineuse (exemple : max à midi, min à minuit)
    lightIntensity = 0.2f + 0.8f * dayNightBlend; // 0.2 = nuit, 1.0 = jour

    // Passe la valeur de blend à la skybox
    skybox.setBlendValue(dayNightBlend);
}

// ----------------------------------------------

void World::addStaticObstacles(const std::vector<Model*>& models, const std::vector<glm::mat4>& modelMatrices)
{
    for (size_t i = 0; i < modelMatrices.size(); ++i) {
        Model* model = models[i % models.size()];

        staticObstacles.push_back(model->collisionBox.getTransformed(modelMatrices[i]));
    }
}
