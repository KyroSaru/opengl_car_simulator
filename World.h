#pragma once

#include <vector>
#include <memory>

#include "Car.h"
#include "Frustum.h"
#include "JoystickManager.h"
#include "Terrain.h"

class World {
public:
    World(int windowWidth, int windowHeight, GLFWwindow* window);

    void load();
    void draw(Shader& shader, Shader& wireframeShader, Shader& terrainShader);
    void update(float deltaTime);
    void calculateViewport(int playerIndex, int& x, int& y, int& w, int& h);

    void addPlayer(const std::string& modelPath, const glm::vec3& startPosition);

private:
    int width, height;
    GLFWwindow* window;

    Terrain terrain;
    std::vector<Car> voitures;

    Frustum frustum;

    // On a pas plus d'un clavier
    std::shared_ptr<Keyboard> keyboard;

    int nbJoueurMax = 4;
};