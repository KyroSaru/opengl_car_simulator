#include "BoundingBox.h"
#include <glad/glad.h>
#include <array>
#include <vector>

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

    // Intersecte ou est à l'intérieur
    return true; 
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
    // Initialise avec le premier coin transformé
    glm::vec3 firstTransformed = glm::vec3(modelMatrix * glm::vec4(corners[0], 1.0f));
    transformedBox.min = firstTransformed;
    transformedBox.max = firstTransformed;

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

// ---------------

// Renvoie les axes locaux d'une bounding box (après transfo.)
static std::array<glm::vec3, 3> getBoundingBoxAxes(const glm::mat4& model) {
    return 
    {
        glm::normalize(glm::vec3(model[0])), // Axe X local
        glm::normalize(glm::vec3(model[1])), // Axe Y local
        glm::normalize(glm::vec3(model[2]))  // Axe Z local
    };
}

// Renvoie les 8 coins d'une bounding box (après transfo.)
static std::array<glm::vec3, 8> getBoundingBoxCorners(const BoundingBox& box, const glm::mat4& model) {
    std::array<glm::vec3, 8> corners = {
        glm::vec3(box.min.x, box.min.y, box.min.z),
        glm::vec3(box.min.x, box.min.y, box.max.z),
        glm::vec3(box.min.x, box.max.y, box.min.z),
        glm::vec3(box.min.x, box.max.y, box.max.z),
        glm::vec3(box.max.x, box.min.y, box.min.z),
        glm::vec3(box.max.x, box.min.y, box.max.z),
        glm::vec3(box.max.x, box.max.y, box.min.z),
        glm::vec3(box.max.x, box.max.y, box.max.z)
    };

	// On transforme les coins avec la matrice modèle
    for (auto& c : corners) 
        c = glm::vec3(model * glm::vec4(c, 1.0f));

    return corners;
}

// Projette tous les coins sur un axe et retourne le min/max projeté (pour savoir où débute/termine la bounding box sur cet axe)
static void projectOntoAxis(const std::array<glm::vec3, 8>& corners, const glm::vec3& axis, float& min, float& max) {
	// Initialisation avec le premier coin
    min = max = glm::dot(corners[0], axis);

    for (int i = 1; i < 8; ++i) {
        // Produit scalaire de chaque coin sur l'axe
        float val = glm::dot(corners[i], axis);
		// MAJ le min/max
        if (val < min) min = val;
        if (val > max) max = val;
    }
}

// Teste la collision de 2 bounding box orientées via SAT
bool BoundingBox::intersectsOBB(const glm::mat4& modelA, const BoundingBox& boxA, const glm::mat4& modelB, const BoundingBox& boxB, float margin) {
	// On récupère les axes des bounding box transformées (car changement d'orientation, rotation)
    auto axesA = getBoundingBoxAxes(modelA);
    auto axesB = getBoundingBoxAxes(modelB);
	// On récupère les coins des bounding box transformées
    auto cornersA = getBoundingBoxCorners(boxA, modelA);
    auto cornersB = getBoundingBoxCorners(boxB, modelB);

    std::vector<glm::vec3> axes;
	// 3 axes de A, 3 axes de B donc 9 croisements possibles
    axes.insert(axes.end(), axesA.begin(), axesA.end());
    axes.insert(axes.end(), axesB.begin(), axesB.end());
    for (const auto& a : axesA)
        for (const auto& b : axesB)
            axes.push_back(glm::normalize(glm::cross(a, b)));

    for (const auto& axis : axes) {
        // Ignore axes nuls
        if (glm::length(axis) < 1e-6f) continue; 
        float minA, maxA, minB, maxB;
        projectOntoAxis(cornersA, axis, minA, maxA);
        projectOntoAxis(cornersB, axis, minB, maxB);
		if (maxA - margin < minB || maxB - margin < minA)
            // Séparation trouvée
            return false; 
    }

    // Pas de séparation, donc collision
    return true; 
}
