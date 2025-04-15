#include "Car.h"
#include <glm/gtx/string_cast.hpp>

Car::Car(const std::string& modelPath)
    : position(0.0f, 0.0f, 0.0f), direction(0.0f, 0.0f, -1.0f)
{
    // Charger le corps de la voiture
    body = Model(modelPath, "Body");

    // Charger les roues
    wheels[0] = Model(modelPath, "Roue_ARD");
    wheels[1] = Model(modelPath, "Roue_ARG");
    wheels[2] = Model(modelPath, "Roue_AVD");
    wheels[3] = Model(modelPath, "Roue_AVG");
}

glm::vec3 Car::getPosition() const 
{
    return position;
}

glm::vec3 Car::getDirection() const 
{
    return direction;
}

void Car::setPosition(const glm::vec3& newPosition) 
{
    position = newPosition;
}

void Car::setDirection(const glm::vec3& newDirection) 
{
    direction = glm::normalize(newDirection);
}

glm::mat4 Car::getBodyModelMatrix() const {
    // Transfos du corps
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position); // Translation à la position correcte (qui est changeante)
    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotation de base pour la mettre "à l'endroit"
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f)); // Homotéthie pour réduire la taille du modèle

    return model;
}


bool Car::isVisible(const Frustum& frustum) const {
    glm::mat4 modelMatrix = getBodyModelMatrix();
    auto bodyTransformedBox = body.boundingBox.getTransformed(modelMatrix);
    if (frustum.isInFrustum(bodyTransformedBox)) return true;

    for (int i = 0; i < 4; ++i) {
        auto wheelTransformedBox = wheels[i].boundingBox.getTransformed(modelMatrix);
        if (frustum.isInFrustum(wheelTransformedBox)) return true;
    }

    return false;
}

void Car::Draw(Shader& shader) 
{
    // Transfos du corps
    glm::mat4 body_model = getBodyModelMatrix();
    std::cout << "Car Model Matrix: " << glm::to_string(body_model) << std::endl;
    shader.setMat4("model", body_model);
    body.Draw(shader);

    // Transfos des roues
    for (int i = 0; i < 4; i++) // 0 : ARD, 1 : ARG, 2 : AVD , 3 : AVG
    {
        glm::mat4 wheel_model = body_model;
        shader.setMat4("model", wheel_model);
        wheels[i].Draw(shader);
    }
}

// -----------------------------------------

void Car::updatePhysics(float deltaTime, GLFWwindow* window)
{
    // Vitesse de déplacement
    float moveSpeed = 5.0f;

    // Avancer
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        position += direction * moveSpeed * deltaTime;
    }

    // Reculer
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        position -= direction * moveSpeed * deltaTime;
    }
}
