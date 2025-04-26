#pragma once

#include <vector>
#include <memory>

#include "Camera.h"
#include "Car.h"
#include "Model.h"
#include "Frustum.h"
#include "JoystickManager.h"

class World {
public:
    World(int windowWidth, int windowHeight, GLFWwindow* window);

    void load();
    void draw(Shader& shader, Shader& wireframeShader);
    void update(float deltaTime);

    Camera& getCamera() { return camera; }

    //void addPlayer(const std::string& modelPath);

private:
    int width, height;
    GLFWwindow* window;

    Camera camera;
    Model terrain;
    Car voiture;

    JoystickManager joystickManager;
    Frustum frustum;

    float lastFrame = 0.0f;
};