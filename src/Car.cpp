#include "Car.h"
#include "BoundingBox.h"
#include <glm/gtx/string_cast.hpp>

int Car::nextId = 0;

Car::Car(const std::string& modelPath)
    : position(0.0f, 2.0f, 0.0f), direction(0.0f, 0.0f, -1.0f), id(nextId++),
    wheelOffsets{
        glm::vec3(1.375f, 0.572f, 3.141f),
        glm::vec3(-1.375f, 0.572f, 3.141f),
        glm::vec3(1.375f, 0.572f, -1.9621f),
        glm::vec3(-1.375f, 0.572f, -1.9604f)
    },
    steeringWheelOffset(0.957f, 0.7f, 0.0f)
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

    // Ajustement manuel des collision box
    body.collisionBox.adjustMin(0.35f, 0.0f, 0.2f);
    body.collisionBox.adjustMax(0.35f, 1.2f, 0.0f);
    wheels[0].collisionBox.adjustMax(1.2f, 0.0f, 2.5f);
    wheels[1].collisionBox.adjustMax(0.0f, 0.0f, 2.5f);
    wheels[1].collisionBox.adjustMin(1.2f, 0.0f, 0.0f);
    wheels[2].collisionBox.adjustMax(1.2f, 0.0f, 0.0f);
    wheels[2].collisionBox.adjustMin(0.0f, 0.0f, 1.315f);
    wheels[3].collisionBox.adjustMin(1.2f, 0.0f, 1.315f);

    // Initialise les hauteur des roues
    for (int i = 0; i < 4; ++i)
        wheelHeights[i] = 0.0f;
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
    glm::mat4 carModel = glm::mat4(1.0f);

    // Translation à la position correcte (qui est changeante)
    carModel = glm::translate(carModel, position);

    // Rotation de base pour la mettre "à l'endroit"
    carModel = glm::rotate(carModel, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    // Applique la rotation en fonction de la direction
    float angle = glm::degrees(atan2(direction.x, -direction.z));
    carModel = glm::rotate(carModel, glm::radians(-angle), glm::vec3(0.0f, 1.0f, 0.0f));

    // Homotéthie pour réduire la taille du modèle
    carModel = glm::scale(carModel, glm::vec3(1.0f, 1.0f, 1.0f));

    return carModel;
}

glm::mat4 Car::getBodyModelMatrix() const {
    // Récupérer la matrice init. de la voiture
    glm::mat4 bodyModel = getCarModelMatrix();

	// Calcule la hauteur du corps par rapport aux roues
	float adjustedBodyHeight = calculateBodyHeight();
	// Translation pour ajuster la hauteur du corps
    bodyModel = glm::translate(bodyModel, glm::vec3(0.0f, adjustedBodyHeight, 0.0f));

    // Calcule les angles de rotation
    glm::vec2 bodyAngles = calculateBodyAngles();
    // Rotation avant-arrière (pitch) autour de l'axe X
    bodyModel = glm::rotate(bodyModel, bodyAngles.x, glm::vec3(1.0f, 0.0f, 0.0f));
    // Rotation gauche-droite (roll) autour de l'axe Z
    bodyModel = glm::rotate(bodyModel, bodyAngles.y, glm::vec3(0.0f, 0.0f, 1.0f));

    return bodyModel;
}

glm::mat4 Car::getWheelModelMatrix(int i, glm::vec2 bodyAngles) const
{
    glm::mat4 wheel_model = getCarModelMatrix();

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

    return wheel_model;
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

    // Transfert de poids dynamique (WIP)
    if (weightTransferEnabled) {
        return glm::vec2(-pitchAngle + weightTransferPitch, rollAngle + weightTransferRoll);
    }

    return glm::vec2(-pitchAngle, rollAngle);
}

float Car::calculateBodyHeight() const {
    // Calcul et application des rotations au châssis (pitch et roll)
    glm::vec2 angles = calculateBodyAngles();
    glm::mat4 rot = glm::rotate(glm::mat4(1.0f), angles.x, glm::vec3(1.0f, 0.0f, 0.0f));
    rot = glm::rotate(rot, angles.y, glm::vec3(0.0f, 0.0f, 1.0f));

	// Rotation appliquée aux offsets des essieux (moyenne des offsets des deux roues avant/arrière)
    glm::vec3 frontOffset = (glm::vec3(rot * glm::vec4(wheelOffsets[2], 1.0f)) + glm::vec3(rot * glm::vec4(wheelOffsets[3], 1.0f))) / 2.0f;
    glm::vec3 rearOffset = (glm::vec3(rot * glm::vec4(wheelOffsets[0], 1.0f)) + glm::vec3(rot * glm::vec4(wheelOffsets[1], 1.0f))) / 2.0f;
	// Hauteurs des essieux (moyenne des hauteurs des deux roues avant/arrière)
    float frontHeight = (wheelHeights[2] + wheelHeights[3]) / 2.0f;
    float rearHeight = (wheelHeights[0] + wheelHeights[1]) / 2.0f;
    // Interpolation linéaire de la hauteur
    float interpHeight = rearHeight + (frontHeight - rearHeight) / 2.0f;
    float interpOffset = rearOffset.y + (frontOffset.y - rearOffset.y) / 2.0f;

    return interpHeight - interpOffset - bodyToWheelDistance + bodyHeightOffset;
}

// -----------------------------------------------

bool Car::isVisible(const Frustum& frustum) const {
    glm::mat4 modelMatrix = getBodyModelMatrix();
    auto bodyTransformedBox = body.frustumBox.getTransformed(modelMatrix);

    if (frustum.isInFrustum(bodyTransformedBox)) return true;

    for (int i = 0; i < 4; ++i) {
        auto wheelTransformedBox = wheels[i].frustumBox.getTransformed(modelMatrix);
        if (frustum.isInFrustum(wheelTransformedBox)) return true;
    }

    return false;
}

void Car::Draw(Shader& carShader, Shader& phareShader)
{
    // Transfos du corps de la voiture
    glm::mat4 bodyModel = getBodyModelMatrix();

    // std::cout << "Car Body Model Matrix: " << glm::to_string(body_model) << std::endl;
    carShader.Activate();

    // [MATERIAUX]
    carShader.setVec3("ambientColor", carsColors.ambientColor);
    carShader.setVec3("diffuseColor", carsColors.diffuseColor);

    // --- [VOLANT] ---
    glm::mat4 steeringWheelModel = bodyModel;
    {
        // Translation au centre du volant
        steeringWheelModel = glm::translate(steeringWheelModel, steeringWheelOffset);
        // Applique l'angle de braquage au volant
        steeringWheelModel = glm::rotate(steeringWheelModel, glm::radians(-steeringAngle * 3.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        // Retour au centre original
        steeringWheelModel = glm::translate(steeringWheelModel, -steeringWheelOffset);
    }
    carShader.setMat4("model", steeringWheelModel);
    steeringWheel.Draw(carShader);

    // --- [CORPS/RETROS/FENETRES] ---
    carShader.setMat4("model", bodyModel);
    body.Draw(carShader);
    leftRearviewMirror.Draw(carShader);
    rightRearviewMirror.Draw(carShader);
    windows.Draw(carShader);

    // --- [PHARES] ---
    phareShader.Activate();
    phareShader.setMat4("model", bodyModel);
    phareShader.setVec3("headlightColor", headlightsOn ? headlightColorOn : headlightColorOff);
    headlights.Draw(phareShader);

    // --- [ROUES] ---
    // Récupérer les angle du corps (angles.x = pitch, angles.y = roll)
    glm::vec2 bodyAngles = calculateBodyAngles();

    // Transfos des roues
    for (int i = 0; i < 4; i++) // 0 : ARD, 1 : ARG, 2 : AVD , 3 : AVG
    {
        glm::mat4 wheelModel = getWheelModelMatrix(i, bodyAngles);

        // std::cout << "Wheel " << i << " Model Matrix: " << glm::to_string(wheel_model) << std::endl;
        carShader.Activate();
        carShader.setMat4("model", wheelModel);
        wheels[i].Draw(carShader);
    }
}

void Car::drawWireframes(Shader& wireframeShader, const glm::mat4& view, const glm::mat4& projection) const
{
    // Active le shader wireframe
    wireframeShader.Activate();
    wireframeShader.setMat4("view", view);
    wireframeShader.setMat4("projection", projection);

    // Dessine la bounding box du corps
    glm::mat4 bodyModel = getBodyModelMatrix();
    wireframeShader.setMat4("model", bodyModel);
    body.collisionBox.drawWireframe(wireframeShader);

    glm::vec2 bodyAngles = calculateBodyAngles();

    for (int i = 0; i < 4; i++)
    {
		// On applique les mêmes transfos que pour le dessin normal pour que les bounding box soient au bon endroit
        glm::mat4 wheelModel = getWheelModelMatrix(i, bodyAngles);

        // Dessine les bounding boxes des roues
        wireframeShader.setMat4("model", wheelModel);
        wheels[i].collisionBox.drawWireframe(wireframeShader);
    }
}

void Car::drawWheelTrail(Shader& trailShader, const glm::mat4& view, const glm::mat4& projection, const std::deque<TrailPoint>& points, float thickness) const 
{
        // Pas besoin de dessiner avec aussi peu de points
        if (points.size() < 5) return;

		// Position et transparence d'un point de la traînée (stocker dans un struct pour éviter de devoir calculer le sizeof pour chacun des attributs pour les buffers)
        struct TrailVertex { glm::vec3 pos; float alpha; };
        std::vector<TrailVertex> vertices;

        // Ajuste la trainée en z pour être bien sous la roue (et pas voir le dessin commencé à se dessiner avant) et direction de la voiture
        const float trailZOffset = 0.3f; 
        glm::vec3 forward = glm::normalize(direction);

        for (size_t i = 0; i < points.size(); ++i) {
            const TrailPoint& pt = points[i];
			// Calcul de la direction du segment par rapport au point précédent
            glm::vec3 trailDir;
            if (i == 0)
                trailDir = glm::normalize(points[1].position - pt.position);
            else
                trailDir = glm::normalize(pt.position - points[i - 1].position);

			// Vecteur perpendiculaire (sur XZ) pour la largeur de la trainée (produit vectoriel normalisé entre la direction de la trainée et l'axe Y)
            glm::vec3 perp = glm::normalize(glm::cross(trailDir, glm::vec3(0, 1, 0)));
			// Gestion de la transparence en fonction du temps (on transforme le temps en alpha / le trailPoint en trailVertex)
            float alpha = 1.0f - (pt.time / trailLifetime);

            // Ajustement de la trainée sur l'axe z (avec la bonne direction) avec notre offset 
            glm::vec3 fixedPos = pt.position + forward * trailZOffset;

            // Pour avoir 2 sommets de chaque côté de la trace
            vertices.push_back({ fixedPos - perp * (thickness * 0.5f), alpha });
            vertices.push_back({ fixedPos + perp * (thickness * 0.5f), alpha });
        }

        // Génère et lie VAO, VBO
        GLuint VAO, VBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
		// Envoie les données au VBO en précisant de draw en dynamique (ajout/suppression de points souvent)
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(TrailVertex), vertices.data(), GL_DYNAMIC_DRAW);
		// Définit les attributs des sommets (position et alpha)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TrailVertex), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(TrailVertex), (void*)(sizeof(glm::vec3)));
		// Active le shader de traînée et envoie les uniforms au shader
        trailShader.Activate();
        trailShader.setMat4("view", view);
        trailShader.setMat4("projection", projection);
        trailShader.setVec3("trailColor", trailColor);
		// Dessine la traînée
        glDrawArrays(GL_TRIANGLE_STRIP, 0, static_cast<GLsizei>(vertices.size()));
        // Délie le VAO et supprime VAO et VBO
        glBindVertexArray(0);
        glDeleteBuffers(1, &VBO);
        glDeleteVertexArrays(1, &VAO);

}

void Car::drawWheelsTrails(Shader& trailShader, const glm::mat4& view, const glm::mat4& projection) const
{
	// Ne pas écrire dans le tampon de profondeur (pour éviter que les trainées se dessinent l'une dans l'autre et que ça fasse des artefacts)
	glDepthMask(GL_FALSE);
        drawWheelTrail(trailShader, view, projection, trailPointsFrontLeft, trailThickness);
        drawWheelTrail(trailShader, view, projection, trailPointsFrontRight, trailThickness);
        drawWheelTrail(trailShader, view, projection, trailPointsRearLeft, trailThickness);
        drawWheelTrail(trailShader, view, projection, trailPointsRearRight, trailThickness);
	glDepthMask(GL_TRUE);
}

// -----------------------------------------

void Car::updatePhysics(float deltaTime, GLFWwindow* window, const Terrain& terrain, const std::vector<Car>& allCars, const std::vector<BoundingBox>& staticObstacles, bool isNoClipEnabled)
{
    // -------- [GESTION TERRAIN COLLISION] --------
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
    
    // -------- [CONTROLEURS] --------

    // Résultats Inputs Controlleurs (Gamepad/Clavier)
    float steerInput = 0.0f;
    float accelInput = 0.0f;

	// Si le mode no clip est activé, on ne prend plus en compte manette/clavier
    if (!isNoClipEnabled) {
        if (gamepad)
        {
            steerInput = gamepad->getLeftStickX(); // stick gauche horizontal
            float r2 = gamepad->getRightTrigger(); // accélération
            float l2 = gamepad->getLeftTrigger();  // frein

            accelInput = r2 - l2; // entre -1 et +1
        }
        else if (keyboard)
        {
            steerInput = keyboard->getSteerInput(window);
            accelInput = keyboard->getAccelInput(window);
        }
    }

    // -------- [TOURNER/ANGLE DE BRAQUAGE] --------
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

    // -------- [ACCELERATION/DECELERATION] --------
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

    // -------- [PERTE DE VITESSE DE L'ANGLE DE BRAQUAGE] --------
    if (std::abs(currentSpeed) > 8.0f) {
        // Facteur entre l'angle de braquage actuelle et maximale (plus le angle de braquage est grand et s'approche de l'angle maximum : plus le véhicule doit ralentir)
        float steeringFactor = std::abs(steeringAngle) / maxSteeringAngle;
		// Force/rapidité à laquelle la voiture perd de la vitesse
		float lossStrength = 1.25f;
        // Calcul la perte de vitesse en fonction du facteur et de la force
        float loss = std::abs(currentSpeed) * steeringFactor * lossStrength * deltaTime;
		// Applique la perte de vitesse si avance
        if (currentSpeed > 0.0f) {
            currentSpeed -= loss;
            if (currentSpeed < 0.0f) currentSpeed = 0.0f;
        }
		// Applique la perte de vitesse si recule
        else {
            currentSpeed += loss;
            if (currentSpeed > 0.0f) currentSpeed = 0.0f;
        }
    }

	// -------- [APPLICATION DE LA PHYSIQUE CHAQUE FRAME] --------
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

        // [GESTION DES COLLISIONS]
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
                    float dynamicBounceBack = glm::clamp(impactSpeed / maxSpeed, 0.2f, 1.0f) * 0.5f;
                    position.x += contactNormal.x * dynamicBounceBack;
                    position.z += contactNormal.z * dynamicBounceBack;
                } 
            }   
        }
    }

    // -------- [TRANSFERT DE POIDS (WorkInProgress)] --------
    // Transfert latéral : + on va vite, + on se penche
	float newRoll = -steeringAngle * (currentSpeed / maxSpeed) * 0.01f; // -steeringAngle pour que le véhicule se penche à l'opposé de la direction où on tourne
	weightTransferRoll += (newRoll - weightTransferRoll) * deltaTime * 5.0f; // interpolé par 5 fois le deltaTime pour que ça soit plus réactif

    // Transfert avant/arrière : dépend de l’accélération/freinage
    float accel = accelInput * acceleration;
    float newPitch = -accel * 0.008f;
    weightTransferPitch += (newPitch - weightTransferPitch) * deltaTime * 5.0f;

    // -------- [TRAINEE DES ROUES] --------
    if (wheelsTrailsEnabled) {
        // MAJ du timer de la trainée
        trailTimer += deltaTime;
        // Si la vitesse est suffisante et que l'intervalle est respectée, on ajoute un point aux trainées
        if (trailTimer >= trailTimeBetweenTwoPoints && std::abs(currentSpeed) > 1.0f) {
            // Calcul de la rotation du véhicule (pour calcul correct des offsets des roues)
            glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(-glm::degrees(atan2(direction.x, -direction.z))), glm::vec3(0.0f, 1.0f, 0.0f));
            // Calcul des offsets des roues pour obtenir leurs positions (x, z)
            glm::vec3 offsetFrontLeft = glm::vec3(rotationMatrix * glm::vec4(wheelOffsets[3], 1.0f));
            glm::vec3 posFrontLeft = position + offsetFrontLeft;
            glm::vec3 offsetFrontRight = glm::vec3(rotationMatrix * glm::vec4(wheelOffsets[2], 1.0f));
            glm::vec3 posFrontRight = position + offsetFrontRight;
            glm::vec3 offsetRearLeft = glm::vec3(rotationMatrix * glm::vec4(wheelOffsets[1], 1.0f));
            glm::vec3 posRearLeft = position + offsetRearLeft;
            glm::vec3 offsetRearRight = glm::vec3(rotationMatrix * glm::vec4(wheelOffsets[0], 1.0f));
            glm::vec3 posRearRight = position + offsetRearRight;

            // Calcul de la hauteur de la trainée (y) par rapport à la hauteur du terrain sous chaque roue
            const float trailYOffset = 0.15f; // on ajuste juste un peu pour éviter d'être dans le sol
            posFrontLeft.y = terrain.getHeightAt(posFrontLeft.x, posFrontLeft.z) + trailYOffset;
            posFrontRight.y = terrain.getHeightAt(posFrontRight.x, posFrontRight.z) + trailYOffset;
            posRearLeft.y = terrain.getHeightAt(posRearLeft.x, posRearLeft.z) + trailYOffset;
            posRearRight.y = terrain.getHeightAt(posRearRight.x, posRearRight.z) + trailYOffset;

            // Ajoute les points au deque
            trailPointsFrontLeft.push_back({ posFrontLeft, 0.0f });
            trailPointsFrontRight.push_back({ posFrontRight, 0.0f });
            trailPointsRearLeft.push_back({ posRearLeft, 0.0f });
            trailPointsRearRight.push_back({ posRearRight, 0.0f });

            // Réinit. le timer
            trailTimer = 0.0f;
        }

        // Gestion de la durée de vie des points des trainées
        // MAJ de temps vécu des points à chaque frame
        for (TrailPoint& pt : trailPointsFrontLeft) pt.time += deltaTime;
        for (TrailPoint& pt : trailPointsFrontRight) pt.time += deltaTime;
        for (TrailPoint& pt : trailPointsRearLeft) pt.time += deltaTime;
        for (TrailPoint& pt : trailPointsRearRight) pt.time += deltaTime;
        // Suppression des points trop vieux
        while (!trailPointsFrontLeft.empty() && trailPointsFrontLeft.front().time > trailLifetime)
            trailPointsFrontLeft.pop_front();
        while (!trailPointsFrontRight.empty() && trailPointsFrontRight.front().time > trailLifetime)
            trailPointsFrontRight.pop_front();
        while (!trailPointsRearLeft.empty() && trailPointsRearLeft.front().time > trailLifetime)
            trailPointsRearLeft.pop_front();
        while (!trailPointsRearRight.empty() && trailPointsRearRight.front().time > trailLifetime)
            trailPointsRearRight.pop_front();
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
