#include "World.h"


World::World(int windowWidth, int windowHeight, GLFWwindow* win)
    : width(windowWidth), height(windowHeight), window(win),
    camera(windowWidth, windowHeight, glm::vec3(0.0f, 0.0f, 2.0f)),
    voiture("models/voiture_gltf/Car.gltf"),
    terrain("models/sol_gltf/sol_gravier.gltf")
{
    camera.setMode(THIRD_PERSON);
}

void World::load()
{

}

void World::update(float deltaTime)
{
    if (!voiture.hasGamepad()) {
        auto gamepad = joystickManager.findGamepad();
        if (gamepad) {
            voiture.setGamepad(gamepad);
            camera.setGamepad(gamepad);
            std::cout << "Manette n°" << gamepad->jid() << " connectée : " << glfwGetJoystickName(gamepad->jid()) << std::endl;
        }
    }
    else if (voiture.hasGamepad() && !joystickManager.isConnected(voiture.getGamepad()->jid())) {
        voiture.setGamepad(nullptr);
        camera.setGamepad(nullptr);
        std::cout << "Manette déconnectée ! Retour clavier.\n";
    }

    if (camera.getMode() == THIRD_PERSON) {
        voiture.updatePhysics(deltaTime, window);
    }

    camera.Inputs(window);
    camera.update(voiture.getPosition(), voiture.getDirection());
}

void World::draw(Shader& shader, Shader& wireframeShader)
{
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.Activate();
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
    glm::mat4 view = camera.getViewMatrix();
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);

    frustum.calculateFrustum(projection, view);

    if (voiture.isVisible(frustum))
    {
        voiture.Draw(shader);
        voiture.DrawWireframes(wireframeShader, view, projection);
    }
}