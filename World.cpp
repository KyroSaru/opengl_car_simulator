#include "World.h"

#include <random>

World::World(int windowWidth, int windowHeight, GLFWwindow* win)
    : width(windowWidth), height(windowHeight), window(win),
    keyboard(std::make_shared<Keyboard>()), 
    terrain("models/terrain/heightmap.png", 35.0f), 
    bois_abandon("models/bois_abandon/bois_abandon.gltf"),
    cactus("models/cactus/cactus.gltf"), 
    pierre1("models/pierre/pierre1.gltf"), pierre2("models/pierre/pierre2.gltf"), pierre3("models/pierre/pierre3.gltf"), 
    bois1("models/bois/bois1.gltf"), bois2("models/bois/bois2.gltf"), bois3("models/bois/bois3.gltf"), 
    herbe1("models/herbe/herbe1.gltf"), herbe2("models/herbe/herbe2.gltf"), herbe3("models/herbe/herbe3.gltf")
{
    JoystickManager::init();
}


void World::loadModels()
{
    addPlayer("models/voiture_gltf/voiture.gltf", glm::vec3(0.0f, 2.0f, 0.0f));

	// G�n�res al�atoirement des mod�les de cactus, pierres, bois et herbe
    generateModels(cactusModelMatrices, 50, 1.5f, 3.0f);
    generateModels(pierreModelMatrices, 60, 0.5f, 2.5f);
    generateModels(boisModelMatrices, 25, 1.5f, 2.5f);
    generateModels(herbeModelMatrices, 40, 0.5f, 1.5f);

}

void World::addPlayer(const std::string& modelPath, const glm::vec3& startPosition) {

    if (voitures.size() >= maxPlayers)
    {
		std::cout << "Maximum number of players reached! [" << maxPlayers << "]\n" << std::endl;
        return;
    }

	// Cr�ation de la voiture & positionnement
    Car car(modelPath);
    car.setPosition(startPosition);

	// Cr�ation de la cam�ra 3rd Person pour la voiture
    auto cam = std::make_shared<Camera>(width, height, startPosition);
    cam->setMode(THIRD_PERSON);
    car.setCamera(cam);

	// Ajout de la voiture � la liste
    voitures.push_back(car);
}

void World::worldInput() {
    if (keyboard->wasEnterPressed(window))
        addPlayer("models/voiture_gltf/voiture.gltf", voitures[voitures.size() - 1].getPosition() + glm::vec3(15.0f, 0.0f, 0.0f)); // pas optimal rien ne nous dit que cette position est libre ...

    if (keyboard->wasBackspacePressed(window)) {
        if (voitures.size() > 1) // Evidemment si il ne reste plus qu'un joueur on ne permet pas le retrait du joueur restant
        {
            auto& car = voitures.back();

            std::cout << "Retrait du joueur n�" << car.getId() + 1 << " de la partie ...\n";

            // Si la voiture a une manette, on d�sasigne la manette
            if (car.hasGamepad())
                car.getGamepad()->unassign();
            // Peut-etre ajouter quelque chose de similair pour le clavier, bon faut etre fort pour deconnecter son clavier ...

            voitures.pop_back();
        }
    }

    // Phares
    if (keyboard->wasHPressed(window)) {
		for (auto& voiture : voitures) {
			voiture.setHeadlightsOn(!voiture.getHeadlightsOn());
			std::cout << "Headlights " << (voiture.getHeadlightsOn() ? "ON" : "OFF") << " for player " << voiture.getId() + 1 << std::endl;
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
                std::cout << "Joueur n�" << voiture.getId() + 1 << " ; Manette n�" << gamepad->jid() << " connect�e : " << glfwGetJoystickName(gamepad->jid()) << std::endl;
            }
            else if (!keyboard->isAssigned())
            {
                keyboard->assign();
                voiture.setKeyboard(keyboard);
                voiture.getCamera()->setGamepad(nullptr); // ajouter une gestion de la souris dynamique
                std::cout << "Joueur " << voiture.getId() + 1 << " utilise le clavier.\n";
            }
        }
        // La voiture a eu un gamepad mais celui-ci a �t� deconnect�
        else if (voiture.hasGamepad() && !JoystickManager::isConnected(voiture.getGamepad()->jid())) {
            auto currentGamepad = voiture.getGamepad();
            currentGamepad->unassign();
            voiture.setGamepad(nullptr);
            voiture.getCamera()->setGamepad(nullptr);
            std::cout << "Manette d�connect�e ! Retour clavier.\n";
        }
    }
}

void World::updateScene(float deltaTime)
{
    // Met � jour la direction de la lumi�re
    updateLightDirection(deltaTime);

    // Cherche tous les gamepads disponibles
    bindController();
    // v�rifie si le joueurs demande l'ajout d'un autre joueur
    worldInput();


    for (auto& voiture : voitures) {
        // Physique pris en compte qu'en 3rd Person
        if (voiture.getCamera()->getMode() == THIRD_PERSON) {
            voiture.updatePhysics(deltaTime, window, terrain);
        }

        // Gestion des touches et m�j de la cam�ra
        voiture.getCamera()->Update(voiture.getBodyPosition(), voiture.getDirection(), terrain);
        voiture.getCamera()->Inputs(window);
    }

    // V�rif. des collisions
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
            // Plein �cran
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
                y = (1 - (playerIndex / 2)) * h; // inverse l'ordre vertical (1er en haut � gauche)
          break;
    }
}

void World::renderScene(Shader& shader, Shader& phareShader, Shader& wireframeShader, Shader& terrainShader, Shader& defaultShader)
{
    // D�finit la couleur de fond de la fen�tre (RGBA)
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    // Nettoie le back buffer et le depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // V�rifie si on passe en No Clip
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
        Draw(shader, phareShader, wireframeShader, terrainShader, defaultShader, width, height, noClip->getViewMatrix(), noClip->getPosition());
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
            Draw(shader, phareShader, wireframeShader, terrainShader, defaultShader, w, h, camera->getViewMatrix(), camera->getPosition());

            glDisable(GL_SCISSOR_TEST);
        }
    }
}

void World::Draw(Shader& shader, Shader& phareShader, Shader& wireframeShader, Shader& terrainShader, Shader& defaultShader, int viewportWidth, int viewportHeight, const glm::mat4& view, const glm::vec3& viewPos)
{
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)viewportWidth / (float)viewportHeight, 0.1f, 200.0f);
	// Calcul du frustum � partir de la matrice de proj. et de vue
    frustum.calculateFrustum(projection, view);

    // --- [VOITURE] ---
    shader.Activate();
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);
    
    shader.setVec3("lightDir", lightDir);
    shader.setVec3("lightColor", lightColor);
    shader.setVec3("ambientColor", carAmbientColor);
    shader.setVec3("diffuseColor", carDiffuseColor);

    phareShader.Activate();
    phareShader.setMat4("projection", projection);
    phareShader.setMat4("view", view);

    for (auto& car : voitures) {
        if (car.isVisible(frustum)) {
            car.Draw(shader, phareShader);
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
    defaultShader.setVec3("lightDir", lightDir);
    defaultShader.setVec3("lightColor", lightColor);
    // Cactus
    renderModels(defaultShader, frustum, { &cactus }, cactusModelMatrices, cactusAmbientColor, cactusDiffuseColor);
    // Pierres
    renderModels(defaultShader, frustum, { &pierre1, &pierre2, &pierre3 }, pierreModelMatrices, pierreAmbientColor, pierreDiffuseColor);
    // Bois
    renderModels(defaultShader, frustum, { &bois1, &bois2, &bois3 }, boisModelMatrices, boisAmbientColor, boisDiffuseColor);
    // Herbes
    renderModels(defaultShader, frustum, { &herbe1, &herbe2, &herbe3 }, herbeModelMatrices, herbeAmbientColor, herbeDiffuseColor);

    // -- [STRUCTURE ABANDONNEE] ---
    glm::mat4 structModel = glm::mat4(1.0f);
    structModel = glm::translate(structModel, glm::vec3(0.0f, -6.0f, 4.0f));
    structModel = glm::rotate(structModel, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    structModel = glm::scale(structModel, glm::vec3(1.5f, 1.5f, 1.5f));
    defaultShader.setMat4("model", structModel);
    bois_abandon.Draw(defaultShader);
}

// ---------------------------------------------

void World::generateModels(std::vector<glm::mat4>& modelMatrices, int count, float minScale, float maxScale) {
    for (int i = 0; i < count; ++i) {
        // G�n�re des coordonn�es al�atoires (x, z)
        float x = static_cast<float>(rand() % static_cast<int>(terrain.getWidth() * terrain.getScale())) - terrain.getWidth() * terrain.getScale() / 2.0f;
        float z = static_cast<float>(rand() % static_cast<int>(terrain.getHeight() * terrain.getScale())) - terrain.getHeight() * terrain.getScale() / 2.0f;
        // R�cup�re la hauteur du terrain � cette position
        float y = terrain.getHeightAt(x, z);

        // G�n�re une �chelle et une rotation al�atoires
        float scale = minScale + static_cast<float>(rand()) / RAND_MAX * (maxScale - minScale);
        float rotation = static_cast<float>(rand() % 360);

        // Matrice Model du mod�le
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x, y - 20.0f, z));
        model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(scale));

        // Stocke la matrice mod�le
        modelMatrices.push_back(model);
    }
}

void World::renderModels(Shader& shader, const Frustum& frustum, const std::vector<Model*>& models, const std::vector<glm::mat4>& modelMatrices, const glm::vec3& ambientColor, const glm::vec3& diffuseColor)
{
    shader.Activate();

    // Mat�riau du mod�le
    shader.setVec3("ambientColor", ambientColor);
	shader.setVec3("diffuseColor", diffuseColor);

	for (size_t i = 0; i < modelMatrices.size(); ++i) {
		Model* model = models[i % models.size()];

		// V�rifie si le mod�le est visible dans le frustum
		if (model->isVisible(frustum, modelMatrices[i])) {
			shader.setMat4("model", modelMatrices[i]);
			model->Draw(shader);
		}
	}
}

// ---------------------------------------------

void World::updateLightDirection(float deltaTime) 
{
	// Applique la rotation � la direction de la lumi�re
	lightDir = glm::rotate(lightDir, 0.5f * deltaTime, glm::vec3(0.0f, 1.0f, 0.0f));
}
