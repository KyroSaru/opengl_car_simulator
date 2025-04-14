#include "BoundingBox.h"

// Constructeur par d�faut
BoundingBox::BoundingBox() 
    : min(glm::vec3(0.0f)), max(glm::vec3(0.0f)) {}

// Constructeur avec des valeurs initiales
BoundingBox::BoundingBox(const glm::vec3& min, const glm::vec3& max) 
    : min(min), max(max) {}

// Met � jour la bounding box avec un point
void BoundingBox::update(const glm::vec3& point) {
    min = glm::min(min, point);
    max = glm::max(max, point);
}

// V�rifie si la bounding box intersecte un plan
bool BoundingBox::intersects(const glm::vec4& plane) const {
    glm::vec3 normal(plane.x, plane.y, plane.z);
    float distance = plane.w;

    // V�rifie si le sommet le plus proche est en dehors du plan
    if (glm::dot(normal, getPositiveVertex(normal)) + distance < 0) {
        return false; // Compl�tement en dehors
    }

    return true; // Intersecte ou est � l'int�rieur
}

// Retourne le sommet le plus proche d'un plan
glm::vec3 BoundingBox::getPositiveVertex(const glm::vec3& normal) const {
    glm::vec3 result = min;

    if (normal.x >= 0) result.x = max.x;
    if (normal.y >= 0) result.y = max.y;
    if (normal.z >= 0) result.z = max.z;

    return result;
}

// Retourne le sommet le plus �loign� d'un plan
glm::vec3 BoundingBox::getNegativeVertex(const glm::vec3& normal) const {
    glm::vec3 result = max;

    if (normal.x >= 0) result.x = min.x;
    if (normal.y >= 0) result.y = min.y;
    if (normal.z >= 0) result.z = min.z;

    return result;
}