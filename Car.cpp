#include "Car.h"
#include "BoundingBox.h"
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

    // Initialiser les positions locales des roues (calculer via blender + screen) | A REVOIR POUR EVITER QUE LES ROUES AIENT L'AIR DE SAUTILLER
    wheelOffsets[0] = glm::vec3(1.375f, 0.572f, 3.141f);
    wheelOffsets[1] = glm::vec3(-1.375f, 0.572f, 3.141f);
    wheelOffsets[2] = glm::vec3(1.375f, 0.572f, -1.9615f);
    wheelOffsets[3] = glm::vec3(-1.375f, 0.572f, -1.9615f);

    // Ajustement manuel des bounding box pour épouser correctement les roues (pour collision plus tard)
    wheels[0].boundingBox.adjustMax(1.2f, 0.0f, 2.5f);
    wheels[1].boundingBox.adjustMax(0.0f, 0.0f, 2.5f);
    wheels[1].boundingBox.adjustMin(1.2f, 0.0f, 0.0f);
    wheels[2].boundingBox.adjustMax(1.2f, 0.0f, 0.0f);
    wheels[2].boundingBox.adjustMin(0.0f, 0.0f, 1.315f);
    wheels[3].boundingBox.adjustMin(1.2f, 0.0f, 1.315f);
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

std::shared_ptr<Gamepad> Car::getGamepad() { return _gamepad;  }
void Car::setGamepad(std::shared_ptr<Gamepad> g) { _gamepad = g; }
bool Car::hasGamepad() const { return _gamepad != nullptr; }

glm::mat4 Car::getBodyModelMatrix() const {
    // [Transfos du corps]
    glm::mat4 model = glm::mat4(1.0f);

    // Translation à la position correcte (qui est changeante)
    model = glm::translate(model, position);

    // Rotation de base pour la mettre "à l'endroit"
    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    // Calcul l'angle de la direction quand on tourne et applique la rotation en fonction de celle-ci
    float angle = glm::degrees(atan2(direction.x, -direction.z));
    model = glm::rotate(model, glm::radians(-angle), glm::vec3(0.0f, 1.0f, 0.0f));
    
    // Homotéthie pour réduire la taille du modèle
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f)); 

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
    //std::cout << "Car Model Matrix: " << glm::to_string(body_model) << std::endl;
    shader.setMat4("model", body_model);
    body.Draw(shader);


    // Transfos des roues
    for (int i = 0; i < 4; i++) // 0 : ARD, 1 : ARG, 2 : AVD , 3 : AVG
    {
        glm::mat4 wheel_model = body_model;

        // Translation au centre de la roue
        wheel_model = glm::translate(wheel_model, -wheelOffsets[i]);

        // Appliquer l'angle de braquage uniquement aux roues avant
        if (i == 2 || i == 3)
        {
            wheel_model = glm::rotate(wheel_model, glm::radians(steeringAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        }

        wheel_model = glm::rotate(wheel_model, glm::radians(wheelRotationAngle), glm::vec3(1.0f, 0.0f, 0.0f));

        // Retour au centre original
        wheel_model = glm::translate(wheel_model, wheelOffsets[i]);

        //std::cout << "Wheel " << i << " Model Matrix: " << glm::to_string(wheel_model) << std::endl;
        shader.setMat4("model", wheel_model);
        wheels[i].Draw(shader);

       
    }
}

void Car::DrawWireframes(Shader& wireframeShader, const glm::mat4& view, const glm::mat4& projection) const
{
    // Activer le shader wireframe
    wireframeShader.Activate();
    wireframeShader.setMat4("view", view);
    wireframeShader.setMat4("projection", projection);

    // Dessiner la bounding box du corps
    glm::mat4 body_model = getBodyModelMatrix();
    wireframeShader.setMat4("model", body_model);
    body.boundingBox.drawWireframe(wireframeShader);

    for (int i = 0; i < 4; i++) // 0 : ARD, 1 : ARG, 2 : AVD , 3 : AVG
    {
        glm::mat4 wheel_model = body_model;

        // Translation au centre de la roue
        wheel_model = glm::translate(wheel_model, -wheelOffsets[i]);

        // Appliquer l'angle de braquage uniquement aux roues avant
        if (i == 2 || i == 3)
        {
            wheel_model = glm::rotate(wheel_model, glm::radians(steeringAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        }

        // Retour au centre original
        wheel_model = glm::translate(wheel_model, wheelOffsets[i]);

        // Dessiner les bounding boxes des roues
        wireframeShader.setMat4("model", wheel_model);
        wheels[i].boundingBox.drawWireframe(wireframeShader);
    }
}


// -----------------------------------------

void Car::updatePhysics(float deltaTime, GLFWwindow* window)
{
    // Vitesse de braquage des roues
    float steeringSpeed = 65.0f;

    float steerInput = 0.0f;
    float accelInput = 0.0f;

    // si une manette est connectée on gère les contrôles avec
    if (_gamepad)
    {
        steerInput = _gamepad->getLeftStickX(); // stick gauche horizontal
        float r2 = _gamepad->getRightTrigger(); // accélération
        float l2 = _gamepad->getLeftTrigger();  // frein

        accelInput = r2 - l2; // entre -1 et +1
    }
    else
    {
        // [TOURNER/ANGLE DE BRAQUAGE]
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            steerInput = 1.0f;
        }
        else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            steerInput = -1.0f;
        }

        // [ACCELERATION/DECELERATION]
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            accelInput = 1.0f;
        }
        else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            accelInput = -1.0f;
        }
    }

    // Appliquer le braquage
    if (steerInput != 0.0f)
    {
        steeringAngle += steerInput * steeringSpeed * deltaTime;
        // Pour ne pas dépasser l'angle max de braquage
        steeringAngle = std::min(steeringAngle, maxSteeringAngle);
        steeringAngle = std::max(steeringAngle, -maxSteeringAngle);
    }
    else
    {
        // Vitesse à laquelle les roues reviennent droite
        float returnSpeed = 25.0f;
        if (steeringAngle > 0.0f)
        {
            steeringAngle -= returnSpeed * deltaTime;
            if (steeringAngle < 0.0f) steeringAngle = 0.0f;
        }
        else if (steeringAngle < 0.0f)
        {
            steeringAngle += returnSpeed * deltaTime;
            if (steeringAngle > 0.0f) steeringAngle = 0.0f;
        }
    }

    // Appliquer l'accélération ou la décélération
    if (accelInput != 0.0f)
    {
        currentSpeed += accelInput * acceleration * deltaTime;
        // Marche arrière plus lente que marche avant
        currentSpeed = std::min(currentSpeed, maxSpeed);
        currentSpeed = std::max(currentSpeed, -maxSpeed / 2.0f);
    }
    else
    {
        // Décélération progressive (frein moteur)
        if (currentSpeed > 0.0f)
        {
            currentSpeed -= deceleration * deltaTime;
            if (currentSpeed < 0.0f) currentSpeed = 0.0f;
        }
        else if (currentSpeed < 0.0f)
        {
            currentSpeed += deceleration * deltaTime;
            if (currentSpeed > 0.0f) currentSpeed = 0.0f;
        }
    }

    // Calcul la distance par rapport à la vitesse et au temps entre 2 frames
    float distance = currentSpeed * deltaTime;

    // Calcul de la vitesse de rotation de la roue par rapport à la distance
    wheelRotationAngle += (distance / wheelRadius) * (180 / glm::pi<float>());

    // Rotation et déplacement (maintenant qu'on a angle de braquage et vitesse courante)
    if (std::abs(currentSpeed) > 0.001f)
    {
        float steeringRad = glm::radians(steeringAngle);
        // Angle de braquage quasi nulle, on va juste tout droit
        if (std::abs(steeringAngle) < 0.01f)
        {
            position += direction * distance;
        }
        else
        {
            // Approximation du rayon de la courbe par rapport à notre angle de braquage nous permettant de trouver notre vitesse angulaire (angle nécessaire pour tourner correctement à notre vitesse)
            float turnRadius = 4.0f / glm::tan(steeringRad); // 4.0f : longueur entre les essieux (déterminé sur approximativ. sur blender à 6.0f mais baissé un peu parce qu'on peut modifier les valeurs!)
            float angularVelocity = (currentSpeed / turnRadius) * deltaTime;
            // On applique la rotation à notre direction et on avance dans la nouvelle direction
            glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angularVelocity, glm::vec3(0.0f, 1.0f, 0.0f));
            direction = glm::normalize(glm::vec3(rotation * glm::vec4(direction, 0.0f)));
            position += direction * distance;
        }
    }
}


