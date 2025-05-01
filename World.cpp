#include "World.h"

World::World(int windowWidth, int windowHeight, GLFWwindow* win)
    : width(windowWidth), height(windowHeight), window(win),
    keyboard(std::make_shared<Keyboard>()), terrain("models/terrain/heightmap.png", 25.0f)
{
}

void World::load()
{
    addPlayer("models/voiture_gltf/voiture.gltf", glm::vec3(-2.5f, 2.0f, 0.0f));
    addPlayer("models/voiture_gltf/voiture.gltf", glm::vec3(2.5f, 2.0f, 0.0f));
    //addPlayer("models/voiture_gltf/voiture.gltf", glm::vec3(-5.0f, 2.0f, 0.0f));
    //addPlayer("models/voiture_gltf/voiture.gltf", glm::vec3(5.0f, 2.0f, 0.0f));
}

void World::addPlayer(const std::string& modelPath, const glm::vec3& startPosition) {

    if(voitures.size() >= nbJoueurMax)
    {
        std::cout << "Nombre maximum de joueurs atteint [" << nbJoueurMax << "]\n";
        return;
    }

    Car car(modelPath);
    car.setPosition(startPosition);

    // une voiture = une camera
    auto cam = std::make_shared<Camera>(width, height, startPosition);
    cam->setMode(THIRD_PERSON);
    
    car.setCamera(cam);
    voitures.push_back(car);
}

void World::update(float deltaTime)
{
    // On cherche tous les gamepads disponibles
    auto availableGamepads = JoystickManager::findAllAvailableGamepads();

    for (auto& voiture : voitures) {
        // la voiture n'a ni clavier ni gamepad
        if (!voiture.hasGamepad() && !voiture.hasKeyboard()) { // remplacer pas isAssigned ?
            if (!keyboard->isAssigned())
            {
                keyboard->assign();
                voiture.setKeyboard(keyboard);
                voiture.getCamera()->setGamepad(nullptr); // ajouter une gestion de la souris dynamique
                std::cout << "Joueur " << voiture.getId() << " utilise le clavier.\n";
            }
            else if (!availableGamepads.empty()) {
                auto gamepad = availableGamepads.back();
                availableGamepads.pop_back();

                gamepad->assign();
                voiture.setGamepad(gamepad);
                voiture.getCamera()->setGamepad(gamepad);
                std::cout << "Joueur n°" << voiture.getId() << " ; Manette n°" << gamepad->jid() << " connectée : " << glfwGetJoystickName(gamepad->jid()) << std::endl;
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

    for (auto& voiture : voitures) {
        if (voiture.getCamera()->getMode() == THIRD_PERSON) {
            voiture.updatePhysics(deltaTime, window, terrain);
        }

        voiture.getCamera()->Inputs(window);
        voiture.getCamera()->update(voiture.getBodyPosition(), voiture.getDirection(), terrain);
    }
}

void World::calculateViewport(int playerIndex, int& x, int& y, int& w, int& h) {
    switch (voitures.size())
    {
    case 1:
        // plein écran
        x = 0;
        y = 0;
        w = width;
        h = height;
        break;
    case 2:
        // split vertical
        w = width / 2;
        h = height;
        x = (playerIndex == 0) ? 0 : w;
        y = 0;
        break;
    case 3:
        // split vertical
        // puis split horizontal de la partie haute
        if (playerIndex == 0)
        {
            // haut à gauche
            w = width / 2;
            h = height / 2;
            x = 0;
            y = height / 2;
        }
        else if (playerIndex == 1)
        {
            // haut à droite
            w = width / 2;
            h = height / 2;
            x = width / 2;
            y = height / 2;
        }
        else
        {
            // toute la moitié inférieure
            w = width;
            h = height / 2;
            x = 0;
            y = 0;
        }
        break;
    case 4:
        // split horiontal et vertical 
        w = width / 2;
        h = height / 2;
        x = (playerIndex % 2) * w;
        y = (1 - (playerIndex / 2)) * h; // inverse l'ordre vertical
        break;
    }
}

void World::draw(Shader& shader, Shader& wireframeShader, Shader& terrainShader)
{
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int n = static_cast<int>(voitures.size());

    for (int i = 0; i < voitures.size(); ++i)
    {
        // division de l'écran en fonction du nombre de joueur
        
        int x, y, w, h;
        calculateViewport(i, x, y, w, h);

        // délimite la zone du joueur sur l'écran
        glViewport(x, y, w, h);
        glEnable(GL_SCISSOR_TEST);
        glScissor(x, y, w, h);

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)w / (float)h, 0.1f, 200.0f);
        glm::mat4 view = voitures[i].getCamera()->getViewMatrix();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);

        frustum.calculateFrustum(projection, view); // Frustum culling basé sur cette vue

        // --- 2.3. Rendu des voitures (Shader normal) ---
        shader.Activate();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);

        for (auto& car : voitures)
        {
            if (car.isVisible(frustum))
            {
                car.Draw(shader);
            }
        }

        // --- 2.4. Rendu des Wireframes (Shader wireframe) ---
        
        wireframeShader.Activate();
        wireframeShader.setMat4("projection", projection);
        wireframeShader.setMat4("view", view);

        for (auto& car : voitures)
        {
            if (car.isVisible(frustum))
            {
                car.DrawWireframes(wireframeShader, view, projection);
            }
        }


        // --- 2.5. Rendu du terrain ---
        // Gestion du terrain ici pour le moment
        terrainShader.Activate();
        terrainShader.setMat4("view", view);
        terrainShader.setMat4("projection", projection);
        glm::mat4 terrainModel = glm::mat4(1.0f);
        terrainModel = glm::translate(terrainModel, glm::vec3(0.0f, -10.0f, 0.0f));
        terrain.setModelMatrix(terrainModel);
        for (auto& car : voitures)
        {
            terrainShader.setVec3("viewPos", car.getCamera()->getPosition()); // Position de la caméra pour shader
            terrain.Draw(terrainShader);
        }

        glDisable(GL_SCISSOR_TEST);
    }
}