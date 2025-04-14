#include "Frustum.h"

// Calcule les plans du frustum
void Frustum::calculateFrustum(const glm::mat4& projection, const glm::mat4& view) {
    glm::mat4 clip = projection * view;

    // Extraire les 6 plans du frustum
    planes[0] = glm::vec4(clip[0][3] + clip[0][0], clip[1][3] + clip[1][0], clip[2][3] + clip[2][0], clip[3][3] + clip[3][0]); // Plan gauche
    planes[1] = glm::vec4(clip[0][3] - clip[0][0], clip[1][3] - clip[1][0], clip[2][3] - clip[2][0], clip[3][3] - clip[3][0]); // Plan droit
    planes[2] = glm::vec4(clip[0][3] + clip[0][1], clip[1][3] + clip[1][1], clip[2][3] + clip[2][1], clip[3][3] + clip[3][1]); // Plan bas
    planes[3] = glm::vec4(clip[0][3] - clip[0][1], clip[1][3] - clip[1][1], clip[2][3] - clip[2][1], clip[3][3] - clip[3][1]); // Plan haut
    planes[4] = glm::vec4(clip[0][3] + clip[0][2], clip[1][3] + clip[1][2], clip[2][3] + clip[2][2], clip[3][3] + clip[3][2]); // Plan proche
    planes[5] = glm::vec4(clip[0][3] - clip[0][2], clip[1][3] - clip[1][2], clip[2][3] - clip[2][2], clip[3][3] - clip[3][2]); // Plan éloigné

    // Normaliser les plans
    for (int i = 0; i < 6; i++) {
        float length = glm::length(glm::vec3(planes[i]));
        planes[i] /= length;
    }
}

// Vérifie si une boîte englobante est dans le frustum
bool Frustum::isInFrustum(const BoundingBox& box) const {
    for (int i = 0; i < 6; i++) {
        glm::vec3 normal(planes[i].x, planes[i].y, planes[i].z);
        float distance = planes[i].w;

        // Vérifie si la boîte est complètement en dehors d'un plan
        if (glm::dot(normal, box.getPositiveVertex(normal)) + distance < 0) {
            return false; // En dehors du frustum
        }
    }
    return true; // Dans le frustum
}

std::vector<Model*> Frustum::getVisibleModels(const std::vector<Model*>& models) const {
    std::vector<Model*> visibleModels;
    for (const auto& model : models) {
        if (isInFrustum(model->boundingBox)) {
            visibleModels.push_back(model);
        }
    }
    return visibleModels;
}
