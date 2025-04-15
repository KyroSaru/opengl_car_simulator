#include "BoundingBox.h"

// Constructeur par défaut
BoundingBox::BoundingBox() 
    : min(glm::vec3(0.0f)), max(glm::vec3(0.0f)) {}

// Constructeur avec des valeurs initiales
BoundingBox::BoundingBox(const glm::vec3& min, const glm::vec3& max) 
    : min(min), max(max) {}

// Met à jour la bounding box avec un point
void BoundingBox::update(const glm::vec3& point) {
    min = glm::min(min, point);
    max = glm::max(max, point);
}

// Vérifie si la bounding box intersecte un plan
bool BoundingBox::intersects(const glm::vec4& plane) const {
    glm::vec3 normal(plane.x, plane.y, plane.z);
    float distance = plane.w;

    // Vérifie si le sommet le plus proche est en dehors du plan
    if (glm::dot(normal, getPositiveVertex(normal)) + distance < 0) {
        return false;
    }

    return true; // Intersecte ou est à l'intérieur
}

// Applique les transfos de la model matrix à la Bounding Box
BoundingBox BoundingBox::getTransformed(const glm::mat4& modelMatrix) const {
    // Coins de la Bounding Box
    glm::vec3 corners[8] = {
        glm::vec3(min.x, min.y, min.z),
        glm::vec3(min.x, min.y, max.z),
        glm::vec3(min.x, max.y, min.z),
        glm::vec3(min.x, max.y, max.z),
        glm::vec3(max.x, min.y, min.z),
        glm::vec3(max.x, min.y, max.z),
        glm::vec3(max.x, max.y, min.z),
        glm::vec3(max.x, max.y, max.z)
    };

    BoundingBox transformedBox;
    // Transforme chaque coin de la Box avec la model matrix et la met à jour avec le nouveau coin obtenu
    for (int i = 0; i < 8; ++i) {
        glm::vec4 transformed = modelMatrix * glm::vec4(corners[i], 1.0f);
        transformedBox.update(glm::vec3(transformed));
    }

    return transformedBox;
}



// Retourne le sommet le plus proche d'un plan
glm::vec3 BoundingBox::getPositiveVertex(const glm::vec3& normal) const {
    glm::vec3 result = min;

    if (normal.x >= 0) result.x = max.x;
    if (normal.y >= 0) result.y = max.y;
    if (normal.z >= 0) result.z = max.z;

    return result;
}
