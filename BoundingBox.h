#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include <glm/glm.hpp>

class BoundingBox {
public:
    glm::vec3 min; // Coin inf�rieur gauche (la plus proche de l'origine)
    glm::vec3 max; // Coin sup�rieur droit (la plus �loign� de l'origine)

    // Constructeur par d�faut
    BoundingBox();
    // Constructeur avec des valeurs initiales
    BoundingBox(const glm::vec3& min, const glm::vec3& max);

    // Met � jour la bounding box avec un point
    void update(const glm::vec3& point);

    // V�rifie si la bounding box intersecte un plan
    bool intersects(const glm::vec4& plane) const;

    // Applique les transfos de la model matrix � la Bounding Box
    BoundingBox getTransformed(const glm::mat4& modelMatrix) const;

    // Retourne le sommet le plus proche d'un plan
    glm::vec3 getPositiveVertex(const glm::vec3& normal) const;
};

#endif // BOUNDINGBOX_H