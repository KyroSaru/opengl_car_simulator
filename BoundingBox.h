#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include "Shader.h"
#include <glm/glm.hpp>

class BoundingBox {
public:
    glm::vec3 min; // Coin inférieur gauche (la plus proche de l'origine)
    glm::vec3 max; // Coin supérieur droit (la plus éloigné de l'origine)

    // Constructeur par défaut
    BoundingBox();
    // Constructeur avec des valeurs initiales
    BoundingBox(const glm::vec3& min, const glm::vec3& max);

    // Met à jour la bounding box avec un point
    void update(const glm::vec3& point);

    // Ajustement manuel des Bounding Box
    void adjustMin(float xOffset, float yOffset, float zOffset);
    void adjustMax(float xOffset, float yOffset, float zOffset);

    // Vérifie si la bounding box intersecte un plan
    bool intersects(const glm::vec4& plane) const;

    // Applique les transfos de la model matrix à la Bounding Box
    BoundingBox getTransformed(const glm::mat4& modelMatrix) const;

    // Retourne le sommet le plus proche d'un plan
    glm::vec3 getPositiveVertex(const glm::vec3& normal) const;

    // Afficher la Bounding Box en wireframe pour debug
    void drawWireframe(Shader& shader) const;
};

#endif // BOUNDINGBOX_H