#include "World.h"

#include <random>

World::World(int windowWidth, int windowHeight, GLFWwindow* win)
    : width(windowWidth), height(windowHeight), window(win),
    keyboard(std::make_shared<Keyboard>()), terrain("models/terrain/heightmap.png", 35.0f), cactus("models/cactus/cactus.gltf")
{
    JoystickManager::init();
}


void World::loadModels()
{
    addPlayer("models/voiture_gltf/voiture.gltf", glm::vec3(0.0f, 2.0f, 0.0f));

    // Génère 50 cactus aléatoires
    generateCacti(50);
}

void World::addPlayer(const std::string& modelPath, const glm::vec3& startPosition) {

    if (voitures.size() >= maxPlayers)
    {
		std::cout << "Maximum number of players reached! [" << maxPlayers << "]\n" << std::endl;
        return;
    }

	// Création de la voiture & positionnement
    Car car(modelPath);
    car.setPosition(startPosition);

	// Création de la caméra 3rd Person pour la voiture
    auto cam = std::make_shared<Camera>(width, height, startPosition);
    cam->setMode(THIRD_PERSON);
    car.setCamera(cam);

	// Ajout de la voiture à la liste
    voitures.push_back(car);
}

void World::worldInput() {
    if (keyboard->wasEnterPressed(window))
        addPlayer("models/voiture_gltf/voiture.gltf", voitures[voitures.size() - 1].getPosition() + glm::vec3(15.0f, 0.0f, 0.0f)); // pas optimal rien ne nous dit que cette position est libre ...

    if (keyboard->wasBackspacePressed(window)) {
        if (voitures.size() > 1) // Evidemment si il ne reste plus qu'un joueur on ne permet pas le retrait du joueur restant
        {
            auto& car = voitures.back();

            std::cout << "Retrait du joueur n°" << car.getId() + 1 << " de la partie ...\n";


            // Si la voiture a une manette, on désasigne la manette
            if (car.hasGamepad())
                car.getGamepad()->unassign();
            // Peut-etre ajouter quelque chose de similair pour le clavier, bon faut etre fort pour deconnecter son clavier ...

            voitures.pop_back();
        }
    }
}

void World::bindController() {
    // On cherche tous les gamepads disponibles
    auto availableGamepads = JoystickManager::findAllAvailableGamepads();

    for (auto& voiture : voitures) {
        // la voiture n'a ni clavier ni gamepad
        if (!voiture.hasGamepad() && !voiture.hasKeyboard()) { // remplacer pas isAssigned ?
            if (!availableGamepads.empty()) {
                auto gamepad = availableGamepads.back();
                availableGamepads.pop_back();

                gamepad->assign();
                voiture.setGamepad(gamepad);
                voiture.getCamera()->setGamepad(gamepad);
                std::cout << "Joueur n°" << voiture.getId() + 1 << " ; Manette n°" << gamepad->jid() << " connectée : " << glfwGetJoystickName(gamepad->jid()) << std::endl;
            }
            else if (!keyboard->isAssigned())
            {
                keyboard->assign();
                voiture.setKeyboard(keyboard);
                voiture.getCamera()->setGamepad(nullptr); // ajouter une gestion de la souris dynamique
                std::cout << "Joueur " << voiture.getId() + 1 << " utilise le clavier.\n";
            }
        }
        // La voiture a eu un gamepad mais celui-ci a été deconnecté
        else if (voiture.hasGamepad() && !JoystickManager::isConnected(voiture.getGamepad()->jid())) {
            auto currentGamepad = voiture.getGamepad();
            currentGamepad->unassign();
            voiture.setGamepad(nullptr);
            voiture.getCamera()->setGamepad(nullptr);
            std::cout << "Manette déconnectée ! Retour clavier.\n";
        }
    }
}

void World::updateScene(float deltaTime)
{
    // Met à jour la direction de la lumière
    updateLightDirection(deltaTime);

    // Cherche tous les gamepads disponibles
    bindController();
    // vérifie si le joueurs demande l'ajout d'un autre joueur
    worldInput();


    for (auto& voiture : voitures) {
        // Physique pris en compte qu'en 3rd Person
        if (voiture.getCamera()->getMode() == THIRD_PERSON) {
            voiture.updatePhysics(deltaTime, window, terrain);
        }

        // Gestion des touches et màj de la caméra
        voiture.getCamera()->Update(voiture.getBodyPosition(), voiture.getDirection(), terrain);
        voiture.getCamera()->Inputs(window);
    }

    // Vérif. des collisions
    for (size_t i = 0; i < voitures.size(); ++i) {
        for (size_t j = i + 1; j < voitures.size(); ++j) {
            if (voitures[i].checkCollision(voitures[j])) {
				std::cout << "Collision detected between cars " << voitures[i].getId() << " and " << voitures[j].getId() << "!" << std::endl;
            }
        }
    }
}

void World::calculateViewport(int playerIndex, int& x, int& y, int& w, int& h) {
    switch (voitures.size())
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

void World::renderScene(Shader& shader, Shader& wireframeShader, Shader& terrainShader, Shader& cactusShader)
{
    // Définit la couleur de fond de la fenêtre (RGBA)
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    // Nettoie le back buffer et le depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Vérifie si on passe en No Clip
    for (auto& car : voitures) {
        if (car.getCamera()->getMode() == NO_CLIP) {
            noClip = car.getCamera();
            break;
        }
        noClip = nullptr;
    }

    if (noClip) {
        // [NO CLIP]
        glViewport(0, 0, width, height);
        Draw(shader, wireframeShader, terrainShader, cactusShader, width, height, noClip->getViewMatrix(), noClip->getPosition());
    }
    else {
        // [3RD PERSON]
        for (int i = 0; i < voitures.size(); ++i) {
            int x, y, w, h;
            calculateViewport(i, x, y, w, h);
            glViewport(x, y, w, h);

            glEnable(GL_SCISSOR_TEST);
            glScissor(x, y, w, h);

            auto camera = voitures[i].getCamera();
            Draw(shader, wireframeShader, terrainShader, cactusShader, w, h, camera->getViewMatrix(), camera->getPosition());

            glDisable(GL_SCISSOR_TEST);
        }
    }
}

void World::Draw(Shader& shader, Shader& wireframeShader, Shader& terrainShader, Shader& cactusShader, int viewportWidth, int viewportHeight, const glm::mat4& view, const glm::vec3& viewPos)
{
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)viewportWidth / (float)viewportHeight, 0.1f, 200.0f);
	// Calcul du frustum à partir de la matrice de proj. et de vue
    frustum.calculateFrustum(projection, view);

    // --- [VOITURE] ---
    shader.Activate();
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);

    shader.setVec3("lightDir", lightDir);
    shader.setVec3("lightColor", lightColor);
    shader.setVec3("ambientColor", carAmbientColor);
    shader.setVec3("diffuseColor", carDiffuseColor);

    for (auto& car : voitures) {
        if (car.isVisible(frustum)) {
            car.Draw(shader);
        }
    }

    // Dessine les wireframes des voitures
    /*
    wireframeShader.Activate();
    wireframeShader.setMat4("projection", projection);
    wireframeShader.setMat4("view", view);
    for (auto& car : voitures) {
        if (car.isVisible(frustum)) {
            car.DrawWireframes(wireframeShader, view, projection);
        }
    }
    */

    // --- [TERRAIN] ---
    terrainShader.Activate();
    terrainShader.setMat4("projection", projection);
    terrainShader.setMat4("view", view);
    terrainShader.setVec3("viewPos", viewPos);

    // Infos Soleil
    terrainShader.setVec3("lightDir", lightDir);          // Direction du soleil
    terrainShader.setVec3("lightColor", lightColor);      // Couleur du soleil

    glm::mat4 terrainModel = glm::mat4(1.0f);
    terrainModel = glm::translate(terrainModel, glm::vec3(0.0f, -20.0f, 0.0f));
    terrain.setModelMatrix(terrainModel);
    terrain.Draw(terrainShader);

    // --- [CIEL] ---
    skybox.Draw(view, projection, static_cast<float>(glfwGetTime()));

    // --- [CACTUS] ---
	cactusShader.Activate();
	cactusShader.setMat4("projection", projection);
	cactusShader.setMat4("view", view);

    // Infos Soleil
    cactusShader.setVec3("lightDir", lightDir);          // Direction du soleil
    cactusShader.setVec3("lightColor", lightColor);      // Couleur du soleil
    // Transmet les propriétés du matériau
    cactusShader.setVec3("ambientColor", cactusAmbientColor);  // Couleur ambiante
    cactusShader.setVec3("diffuseColor", cactusDiffuseColor);  // Couleur diffuse

    // Dessine chaque cactus
    for (const auto& model : cactusModelMatrices) {
        if (cactus.isVisible(frustum, model)) {
            cactusShader.setMat4("model", model);
            cactus.Draw(cactusShader);
        }
    }

}

void World::generateCacti(int count) {
    for (int i = 0; i < count; ++i) {
        // Génère des coordonnées aléatoires (x, z)
        float x = static_cast<float>(rand() % static_cast<int>(terrain.getWidth() * terrain.getScale())) - terrain.getWidth() * terrain.getScale() / 2.0f;
        float z = static_cast<float>(rand() % static_cast<int>(terrain.getHeight() * terrain.getScale())) - terrain.getHeight() * terrain.getScale() / 2.0f;

        // Récupère la hauteur du terrain à cette position
        float y = terrain.getHeightAt(x, z);

        // Génère une échelle et une rotation aléatoires
        float scale = 1.5f + static_cast<float>(rand() % 150) / 100.0f; // Entre 1.5 et 3.0
        float rotation = static_cast<float>(rand() % 360); // Entre 0 et 360 degrés

        // Matrice Model des cactus
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x, y - 20.0f, z));
        model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(scale));

        // Stocke la matrice modèle
        cactusModelMatrices.push_back(model);
    }
}

void World::updateLightDirection(float deltaTime) 
{
	// Applique la rotation à la direction de la lumière
	lightDir = glm::rotate(lightDir, 0.5f * deltaTime, glm::vec3(0.0f, 1.0f, 0.0f));
}
