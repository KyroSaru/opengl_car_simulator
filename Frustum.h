#ifndef FRUSTUM_H
#define FRUSTUM_H

#include <glm/glm.hpp>
#include "Model.h"
#include "BoundingBox.h"

class Frustum {
private:
    glm::vec4 planes[6]; // Les 6 plans du frustum

public:
    // Calcule les plans du frustum à partir des matrices projection et vue
    void calculateFrustum(const glm::mat4& projection, const glm::mat4& view);

    // Vérifie si une boîte englobante est dans le frustum
    bool isInFrustum(const BoundingBox& box) const;

    // Retourne les modèles visibles dans le frustum
    std::vector<Model*> getVisibleModels(const std::vector<Model*>& models) const;
};

#endif // FRUSTUM_H