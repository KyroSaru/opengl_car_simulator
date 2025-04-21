#include "BoundingBox.h"
#include <glad/glad.h>

// Constructeur par défaut
BoundingBox::BoundingBox() 
    : min(glm::vec3(0.0f)), max(glm::vec3(0.0f)) 
{
}

// Constructeur avec des valeurs initiales
BoundingBox::BoundingBox(const glm::vec3& min, const glm::vec3& max) 
    : min(min), max(max) {}

// Met à jour la bounding box avec un point
void BoundingBox::update(const glm::vec3& point) 
{
    min = glm::min(min, point);
    max = glm::max(max, point);
}

// Ajustement manuel des Bounding Box
void BoundingBox::adjustMin(float xOffset, float yOffset, float zOffset) {
    min.x += xOffset;
    min.y += yOffset;
    min.z += zOffset;
}
void BoundingBox::adjustMax(float xOffset, float yOffset, float zOffset) {
    max.x -= xOffset;
    max.y -= yOffset;
    max.z -= zOffset;
}

// Vérifie si la bounding box intersecte un plan
bool BoundingBox::intersects(const glm::vec4& plane) const 
{
    glm::vec3 normal(plane.x, plane.y, plane.z);
    float distance = plane.w;

    // Vérifie si le sommet le plus proche est en dehors du plan
    if (glm::dot(normal, getPositiveVertex(normal)) + distance < 0) {
        return false;
    }

    return true; // Intersecte ou est à l'intérieur
}

// Applique les transfos de la model matrix à la Bounding Box
BoundingBox BoundingBox::getTransformed(const glm::mat4& modelMatrix) const 
{
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
glm::vec3 BoundingBox::getPositiveVertex(const glm::vec3& normal) const 
{
    glm::vec3 result = min;

    if (normal.x >= 0) result.x = max.x;
    if (normal.y >= 0) result.y = max.y;
    if (normal.z >= 0) result.z = max.z;

    return result;
}

// Afficher la Bounding Box en wireframe pour debug
void BoundingBox::drawWireframe(Shader& shader) const {
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

    // Indices des lignes de la bounding box
    unsigned int indices[] = {
        0, 1, 1, 3, 3, 2, 2, 0, // Face avant
        4, 5, 5, 7, 7, 6, 6, 4, // Face arrière
        0, 4, 1, 5, 2, 6, 3, 7  // Connecteurs entre avant et arrière
    };

    // Créer un VAO et un VBO pour les coins
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // Charger les coins dans le VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(corners), corners, GL_STATIC_DRAW);

    // Charger les indices dans l'EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Configurer les attributs de vertex
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    // Activer le shader
    shader.Activate();

    // Dessiner les lignes
    glBindVertexArray(VAO);
    glDrawElements(GL_LINES, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_INT, 0);

    // Nettoyer
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}
