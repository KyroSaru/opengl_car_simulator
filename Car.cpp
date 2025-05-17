#include "Car.h"
#include "BoundingBox.h"
#include <glm/gtx/string_cast.hpp>

int Car::nextId = 0;

Car::Car(const std::string& modelPath)
    : position(0.0f, 2.0f, 0.0f), direction(0.0f, 0.0f, -1.0f), id(nextId++)
{
    // Charge le corps de la voiture
    body = Model(modelPath, "Body");

    // Charge phares, retro, volant et fenêtres
    headlights = Model(modelPath, "Phares");
    leftRearviewMirror = Model(modelPath, "Retro_Miroir_G");
    rightRearviewMirror = Model(modelPath, "Retro_Miroir_D");
    steeringWheel = Model(modelPath, "Volant");
    windows = Model(modelPath, "Fenetres");

    // Charge les roues
    wheels[0] = Model(modelPath, "Roue_ARD");
    wheels[1] = Model(modelPath, "Roue_ARG");
    wheels[2] = Model(modelPath, "Roue_AVD");
    wheels[3] = Model(modelPath, "Roue_AVG");

    // Initialise les positions locales des roues (calculer via blender + screen)
    wheelOffsets[0] = glm::vec3(1.375f, 0.572f, 3.141f);
    wheelOffsets[1] = glm::vec3(-1.375f, 0.572f, 3.141f);
    wheelOffsets[2] = glm::vec3(1.375f, 0.572f, -1.9621f); // A AJUSTER (car on dirait qu'elle va partir)
    wheelOffsets[3] = glm::vec3(-1.375f, 0.572f, -1.9604f);

    // Ajustement manuel des collision box
    body.collisionBox.adjustMin(0.35f, 0.0f, 0.2f);
    body.collisionBox.adjustMax(0.35f, 1.2f, 0.0f);
    wheels[0].collisionBox.adjustMax(1.2f, 0.0f, 2.5f);
    wheels[1].collisionBox.adjustMax(0.0f, 0.0f, 2.5f);
    wheels[1].collisionBox.adjustMin(1.2f, 0.0f, 0.0f);
    wheels[2].collisionBox.adjustMax(1.2f, 0.0f, 0.0f);
    wheels[2].collisionBox.adjustMin(0.0f, 0.0f, 1.315f);
    wheels[3].collisionBox.adjustMin(1.2f, 0.0f, 1.315f);

	// Initialise la position local du volant (calculer dans Blender)
    steeringWheelOffset = glm::vec3(0.957f, 0.7f, 0.0f);
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

glm::vec3 Car::getBodyPosition() const {
    glm::vec3 adjustedPosition = position;
    adjustedPosition.y += calculateBodyHeight();
    return adjustedPosition;
}

// -----------------------------------------------

glm::mat4 Car::getCarModelMatrix() const {
    glm::mat4 model = glm::mat4(1.0f);

    // Translation à la position correcte (qui est changeante)
    model = glm::translate(model, position);

    // Rotation de base pour la mettre "à l'endroit"
    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    // Applique la rotation en fonction de la direction
    float angle = glm::degrees(atan2(direction.x, -direction.z));
    model = glm::rotate(model, glm::radians(-angle), glm::vec3(0.0f, 1.0f, 0.0f));

    // Homotéthie pour réduire la taille du modèle
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));

    return model;
}

glm::mat4 Car::getBodyModelMatrix() const {
    // Récupérer la matrice init. de la voiture
    glm::mat4 model = getCarModelMatrix();

	// Calcule la hauteur du corps par rapport aux roues
	float adjustedBodyHeight = calculateBodyHeight();
	// Translation pour ajuster la hauteur du corps
	model = glm::translate(model, glm::vec3(0.0f, adjustedBodyHeight, 0.0f));

    // Calcule les angles de rotation
    glm::vec2 angles = calculateBodyAngles();
    // Rotation avant-arrière (pitch) autour de l'axe X
    model = glm::rotate(model, angles.x, glm::vec3(1.0f, 0.0f, 0.0f));
    // Rotation gauche-droite (roll) autour de l'axe Z
    model = glm::rotate(model, angles.y, glm::vec3(0.0f, 0.0f, 1.0f));

    return model;
}

glm::vec2 Car::calculateBodyAngles() const {
    // Moyenne des hauteurs des roues avants et arrières
    float frontAverageHeight = (wheelHeights[2] + wheelHeights[3]) / 2.0f;
    float rearAverageHeight = (wheelHeights[0] + wheelHeights[1]) / 2.0f;
    // Calcul de l'angle avant-arrière (pitch)
    float pitchAngle = glm::atan(frontAverageHeight - rearAverageHeight, glm::length(wheelOffsets[2] - wheelOffsets[0]));

    // Moyenne des hauteurs des roues gauches et droites
    float leftAverageHeight = (wheelHeights[1] + wheelHeights[3]) / 2.0f;
    float rightAverageHeight = (wheelHeights[0] + wheelHeights[2]) / 2.0f;
    // Calcul de l'angle gauche-droite (roll)
    float rollAngle = glm::atan(leftAverageHeight - rightAverageHeight, glm::length(wheelOffsets[0] - wheelOffsets[1]));

    return glm::vec2(-pitchAngle, rollAngle);
}

float Car::calculateBodyHeight() const {
    // Hauteur moyenne des roues
    float averageWheelHeight = (wheelHeights[0] + wheelHeights[1] + wheelHeights[2] + wheelHeights[3]) / 4.0f;

    // Hauteur approx. du corps
    return averageWheelHeight - bodyToWheelDistance;
}

// -----------------------------------------------

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

void Car::Draw(Shader& carShader, Shader& phareShader)
{
    // Transfos du corps de la voiture
    glm::mat4 body_model = getBodyModelMatrix();
    // Transfos de la voiture
    glm::mat4 init_car_model = getCarModelMatrix();

    // std::cout << "Car Model Matrix: " << glm::to_string(body_model) << std::endl;
    carShader.Activate();

    // --- [VOLANT] ---
    glm::mat4 volant_model = body_model;
    {
        // Translation au centre du volant
        volant_model = glm::translate(volant_model, steeringWheelOffset);
        // Applique l'angle de braquage au volant
        volant_model = glm::rotate(volant_model, glm::radians(-steeringAngle * 3.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        // Retour au centre original
        volant_model = glm::translate(volant_model, -steeringWheelOffset);
    }
    carShader.setMat4("model", volant_model);
    steeringWheel.Draw(carShader);

    // --- [CORPS/RETROS/FENETRES] ---
    carShader.setMat4("model", body_model);
    body.Draw(carShader);
    leftRearviewMirror.Draw(carShader);
    rightRearviewMirror.Draw(carShader);
    windows.Draw(carShader);

    // --- [PHARES] ---
    phareShader.Activate();
    phareShader.setMat4("model", body_model);
    phareShader.setVec3("headlightColor", headlightsOn ? headlightColorOn : headlightColorOff);
    headlights.Draw(phareShader);

    // --- [ROUES] ---
    // Récupérer les angle du corps (angles.x = pitch, angles.y = roll)
    glm::vec2 bodyAngles = calculateBodyAngles();

    // Transfos des roues
    for (int i = 0; i < 4; i++) // 0 : ARD, 1 : ARG, 2 : AVD , 3 : AVG
    {
        glm::mat4 wheel_model = init_car_model;

        // Ajuste la hauteur de la roue en fonction du terrain
        wheel_model = glm::translate(wheel_model, glm::vec3(0.0f, wheelHeights[i] - wheelOffsets[i].y, 0.0f));

        // [ANIMATIONS DES ROUES]
        // Translation au centre de la roue
        wheel_model = glm::translate(wheel_model, -wheelOffsets[i]);
        // Appliquer l'angle de braquage uniquement aux roues avant
        if (i == 2 || i == 3)
        {
            wheel_model = glm::rotate(wheel_model, glm::radians(steeringAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        }
        // Appliquer le même angle de roll du corps aux roues (inclinaison gauche-droite)
        wheel_model = glm::rotate(wheel_model, bodyAngles.y, glm::vec3(0.0f, 0.0f, 1.0f));
        // Rotation des roues en fonction de la distance
        wheel_model = glm::rotate(wheel_model, glm::radians(wheelRotationAngle), glm::vec3(1.0f, 0.0f, 0.0f));
        // Retour au centre original
        wheel_model = glm::translate(wheel_model, wheelOffsets[i]);

        // std::cout << "Wheel " << i << " Model Matrix: " << glm::to_string(wheel_model) << std::endl;
        carShader.Activate();
        carShader.setMat4("model", wheel_model);
        wheels[i].Draw(carShader);
    }
}

void Car::DrawWireframes(Shader& wireframeShader, const glm::mat4& view, const glm::mat4& projection) const
{
    // Active le shader wireframe
    wireframeShader.Activate();
    wireframeShader.setMat4("view", view);
    wireframeShader.setMat4("projection", projection);

    // Dessine la bounding box du corps
    glm::mat4 body_model = getBodyModelMatrix();
    wireframeShader.setMat4("model", body_model);
    body.collisionBox.drawWireframe(wireframeShader);

    for (int i = 0; i < 4; i++)
    {
		// On applique les mêmes transfos que pour le dessin normal pour que les bounding box soient au bon endroit
        glm::mat4 wheel_model = body_model;

        wheel_model = glm::translate(wheel_model, -wheelOffsets[i]);
        if (i == 2 || i == 3)
        {
            wheel_model = glm::rotate(wheel_model, glm::radians(steeringAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        }
        wheel_model = glm::translate(wheel_model, wheelOffsets[i]);

        // Dessine les bounding boxes des roues
        wireframeShader.setMat4("model", wheel_model);
        wheels[i].collisionBox.drawWireframe(wireframeShader);
    }
}

// -----------------------------------------

void Car::updatePhysics(float deltaTime, GLFWwindow* window, const Terrain& terrain, const std::vector<Car>& allCars, const std::vector<BoundingBox>& staticObstacles)
{
    // [GESTION TERRAIN COLLISION]
    // Constante pour l'interpolation (plus petite = plus fluide)
    const float smoothing = 25.0f;

    // Calcule la matrice de rotation du véhicule pour MAJ les offsets des roues
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(-glm::degrees(atan2(direction.x, -direction.z))), glm::vec3(0.0f, 1.0f, 0.0f));

    for (int i = 0; i < 4; ++i) {
		// Applique la rotation aux offsets des roues (sinon lorsque l'on tourne, la hauteur des roues n'est pas correctement calculée car offsets des roues incorrects)
        glm::vec3 rotatedOffset = glm::vec3(rotationMatrix * glm::vec4(wheelOffsets[i], 1.0f));
        
        // Calcul de la hauteur du terrain sous la roue
        glm::vec3 worldWheelPos = position + rotatedOffset;
        float terrainHeight = terrain.getHeightAt(worldWheelPos.x, worldWheelPos.z);
        // Interpolation pour rendre les mouvements plus fluides (suppresion de 0.2f pour éviter que le véhicule semble "flotter")
        wheelHeights[i] += (terrainHeight - wheelHeights[i] - 0.2f) * deltaTime * smoothing;
    }
    
    // ---------------------------------------

    // Vitesse de braquage des roues
    float steeringSpeed = 65.0f;

    // Résultats Inputs Controlleurs (Gamepad/Clavier)
    float steerInput = 0.0f;
    float accelInput = 0.0f;

    if (_gamepad)
    {
        steerInput = _gamepad->getLeftStickX(); // stick gauche horizontal
        float r2 = _gamepad->getRightTrigger(); // accélération
        float l2 = _gamepad->getLeftTrigger();  // frein

        accelInput = r2 - l2; // entre -1 et +1
    }
    else if (_keyboard)
    {
        steerInput = _keyboard->getSteerInput(window);
        accelInput = _keyboard->getAccelInput(window);
    }

    // [TOURNER/ANGLE DE BRAQUAGE]
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

    // [ACCELERATION/DECELERATION]
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

    // Calcul la vitesse de rotation de la roue par rapport à la distance
    wheelRotationAngle += (distance / wheelRadius) * (180 / glm::pi<float>());

    // Rotation et déplacement (maintenant qu'on a angle de braquage et vitesse courante)
    if (std::abs(currentSpeed) > 0.001f)
    {
        float steeringRad = glm::radians(steeringAngle);

        // Approximation du rayon de la courbe par rapport à notre angle de braquage nous permettant de trouver notre vitesse angulaire (angle nécessaire pour tourner correctement à notre vitesse)
        float turnRadius = 4.0f / glm::tan(steeringRad); // 4.0f : longueur entre les essieux (déterminé sur approximativ. sur blender à 6.0f mais baissé un peu parce qu'on peut modifier les valeurs!)
        float angularVelocity = (currentSpeed / turnRadius) * deltaTime;
        // On applique la rotation à notre direction et on avance dans la nouvelle direction
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angularVelocity, glm::vec3(0.0f, 1.0f, 0.0f));
        direction = glm::normalize(glm::vec3(rotation * glm::vec4(direction, 0.0f)));

        // Calcul de la nouvelle position potentielle
        glm::vec3 newPosition = position + direction * distance;

        // Vérifie la collision à la nouvelle position
        if (!wouldCollideAt(newPosition, allCars, staticObstacles, id)) {
            position = newPosition;
        }
        // Collision détectée : effet de bumper
        else  {
            // Calcul de la direction du mouvement
            glm::vec3 moveDir = glm::normalize(direction * currentSpeed);

            // Recherche de la normale de contact avec la box
            glm::vec3 contactNormal(0.0f, 0.0f, 0.0f);
            // Flag pour savoir si on a trouvé une collision avec un obstacle (voiture/obstacle statique)
            bool found = false;

            // Collision avec une autre voiture
            for (const auto& car : allCars) {
                if (car.getId() == id) continue;
                if (BoundingBox::intersectsOBB(getBodyModelMatrix(), body.collisionBox, car.getBodyModelMatrix(), car.body.collisionBox, 0.1f)) {
                    glm::vec3 otherCenter = glm::vec3(car.getBodyModelMatrix()[3]);
                    glm::vec3 myCenter = glm::vec3(getBodyModelMatrix()[3]);
                    contactNormal = glm::normalize(myCenter - otherCenter);
                    found = true;
                    break;
                }
            }
            // Collision avec un obstacle statique
            if (!found) {
                for (const auto& box : staticObstacles) {
                    if (BoundingBox::intersectsOBB(getBodyModelMatrix(), body.collisionBox, glm::mat4(1.0f), box, 0.15f)) {
                        glm::vec3 boxCenter = (box.min + box.max) * 0.5f;
                        glm::vec3 myCenter = glm::vec3(getBodyModelMatrix()[3]);
                        contactNormal = glm::normalize(myCenter - boxCenter);
                        found = true;
                        break;
                    }
                }
            }
            // Si pas de collision trouvé normal = direction du mouvement
            if (!found) {
                contactNormal = -moveDir;
            }

            // Si on essaie de reculer (moveDir et contactNormal opposés), autorise le mouvement
            if (glm::dot(moveDir, contactNormal) > 0.0f) {
                position = newPosition;
            } 
            // Applique l'effet de bounce
            else {
                float impactSpeed = std::abs(currentSpeed);
                float dynamicBounceFactor = glm::clamp(impactSpeed / maxSpeed, 0.2f, 1.0f) * bounceFactor;
                currentSpeed *= -dynamicBounceFactor;

                // Si la vitesse est trop faible, on ne se déplace pas (pour éviter les micro-bounce)
                if (!(std::abs(currentSpeed) < 0.25f)) {
                    float dynamicBounceBack = glm::clamp(impactSpeed / maxSpeed, 0.2f, 1.0f) * bounceBackDistance;
                    position.x += contactNormal.x * dynamicBounceBack;
                    position.z += contactNormal.z * dynamicBounceBack;
                } 
            }   
        }
    }
}

bool Car::wouldCollideAt(const glm::vec3& newPosition, const std::vector<Car>& cars, const std::vector<BoundingBox>& staticObstacles, int selfId) const {
    // Crée une copie de la matrice du corps à la nouvelle position
    glm::mat4 futureModel = getBodyModelMatrix();
    futureModel = glm::translate(futureModel, newPosition - position);

    // Collision avec les autres voitures
    for (const auto& car : cars) {
        if (car.getId() == selfId) continue;
        if (BoundingBox::intersectsOBB(futureModel, body.collisionBox, car.getBodyModelMatrix(), car.body.collisionBox, 0.1f)) {
            return true;
        }
    }
    // Collision avec les obstacles statiques (sauf herbe)
    for (const auto& box : staticObstacles) {
        if (BoundingBox::intersectsOBB(futureModel, body.collisionBox, glm::mat4(1.0f), box, 0.2f)) {
            return true;
        }
    }

	// Pas de collision
    return false;
}
