#pragma once

#include <vector>
#include <memory>

#include "Camera.h"
#include "Car.h"
#include "Frustum.h"
#include "JoystickManager.h"
#include "Keyboard.h"
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

    std::shared_ptr<Keyboard> keyboard;

    Terrain terrain;
    std::vector<Car> voitures;

    Frustum frustum;

    float lastFrame = 0.0f;
    float gravity = 9.807f;

    int nbJoueurMax = 4;
};