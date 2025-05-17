#include "Frustum.h"

void Frustum::calculateFrustum(const glm::mat4& projection, const glm::mat4& view) {
    glm::mat4 clip = projection * view;

    // Extrait les 6 plans du frustum
    planes[0] = glm::vec4(clip[0][3] + clip[0][0], clip[1][3] + clip[1][0], clip[2][3] + clip[2][0], clip[3][3] + clip[3][0]); // Plan gauche
    planes[1] = glm::vec4(clip[0][3] - clip[0][0], clip[1][3] - clip[1][0], clip[2][3] - clip[2][0], clip[3][3] - clip[3][0]); // Plan droit
    planes[2] = glm::vec4(clip[0][3] + clip[0][1], clip[1][3] + clip[1][1], clip[2][3] + clip[2][1], clip[3][3] + clip[3][1]); // Plan bas
    planes[3] = glm::vec4(clip[0][3] - clip[0][1], clip[1][3] - clip[1][1], clip[2][3] - clip[2][1], clip[3][3] - clip[3][1]); // Plan haut
    planes[4] = glm::vec4(clip[0][3] + clip[0][2], clip[1][3] + clip[1][2], clip[2][3] + clip[2][2], clip[3][3] + clip[3][2]); // Plan proche
    planes[5] = glm::vec4(clip[0][3] - clip[0][2], clip[1][3] - clip[1][2], clip[2][3] - clip[2][2], clip[3][3] - clip[3][2]); // Plan éloigné

    // Normalise les plans
    for (int i = 0; i < 6; i++) {
        float length = glm::length(glm::vec3(planes[i]));
        planes[i] /= length;
    }
}

bool Frustum::isInFrustum(const BoundingBox& box) const {
    for (int i = 0; i < 6; i++) {
        // Si la boîte est complètement en dehors d'un plan, elle n'est pas dans le frustum
        if (!box.intersects(planes[i])) {
            return false; 
        }
    }
    return true;
}
