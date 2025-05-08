#include "World.h"

World::World(int windowWidth, int windowHeight, GLFWwindow* win)
    : width(windowWidth), height(windowHeight), window(win),
    keyboard(std::make_shared<Keyboard>()), terrain("models/terrain/heightmap.png", 35.0f)
{
}


void World::loadModels()
{
    addPlayer("models/voiture_gltf/voiture.gltf", glm::vec3(-2.5f, 2.0f, 0.0f));
    addPlayer("models/voiture_gltf/voiture.gltf", glm::vec3(2.5f, 2.0f, 0.0f));
    // addPlayer("models/voiture_gltf/voiture.gltf", glm::vec3(-7.5f, 2.0f, 0.0f));
    // addPlayer("models/voiture_gltf/voiture.gltf", glm::vec3(7.5f, 2.0f, 0.0f));
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

void World::updateScene(float deltaTime)
{
    // Cherche tous les gamepads disponibles
    auto availableGamepads = JoystickManager::findAllAvailableGamepads();

    for (auto& voiture : voitures) {
		// Assigne un gamepad ou un clavier à la voiture (priorité au clavier puis les manettes)
        if (!voiture.hasGamepad() && !voiture.hasKeyboard()) {
            if (!keyboard->isAssigned())
            {
                keyboard->assign();
                voiture.setKeyboard(keyboard);
                voiture.getCamera()->setGamepad(nullptr);
				std::cout << "Player " << voiture.getId() + 1 << " uses the keyboard.\n";
            }
            else if (!availableGamepads.empty()) {
                auto gamepad = availableGamepads.back();
                availableGamepads.pop_back();

                gamepad->assign();
                voiture.setGamepad(gamepad);
                voiture.getCamera()->setGamepad(gamepad);
				std::cout << "Player " << voiture.getId() + 1 << " uses gamepad " << gamepad->jid() + 1 << " : " << glfwGetJoystickName(gamepad->jid()) << std::endl;
            }
             
        }
        // Gamepad mais deconnecté
        else if (voiture.hasGamepad() && !JoystickManager::isConnected(voiture.getGamepad()->jid())) {
            auto currentGamepad = voiture.getGamepad();
            currentGamepad->unassign();
            voiture.setGamepad(nullptr);
            voiture.getCamera()->setGamepad(nullptr);
            std::cout << "Disconnected gamepad! Switching to keyboard.\n";
        }

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

void World::renderScene(Shader& shader, Shader& wireframeShader, Shader& terrainShader)
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
        Draw(shader, wireframeShader, terrainShader, width, height, noClip->getViewMatrix(), noClip->getPosition());
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
            Draw(shader, wireframeShader, terrainShader, w, h, camera->getViewMatrix(), camera->getPosition());

            glDisable(GL_SCISSOR_TEST);
        }
    }
}

void World::Draw(Shader& shader, Shader& wireframeShader, Shader& terrainShader, int viewportWidth, int viewportHeight, const glm::mat4& view, const glm::vec3& viewPos)
{
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)viewportWidth / (float)viewportHeight, 0.1f, 200.0f);
	// Calcul du frustum à partir de la matrice de proj. et de vue
    frustum.calculateFrustum(projection, view);

    // Dessine les voitures
    shader.Activate();
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);
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

    // Dessine le terrain
    terrainShader.Activate();
    terrainShader.setMat4("projection", projection);
    terrainShader.setMat4("view", view);
    terrainShader.setVec3("viewPos", viewPos);
    glm::mat4 terrainModel = glm::mat4(1.0f);
    terrainModel = glm::translate(terrainModel, glm::vec3(0.0f, -20.0f, 0.0f));
    terrain.setModelMatrix(terrainModel);
    terrain.Draw(terrainShader);

    // Dessine le ciel
    skybox.Draw(view, projection, glfwGetTime());
}
