#ifndef FRUSTUM_H
#define FRUSTUM_H

#include <glm/glm.hpp>
#include "Model.h"
#include "BoundingBox.h"

class Frustum {
private:
    // Les 6 plans du frustum
    glm::vec4 planes[6]; 

public:
    // Calcule les plans du frustum à partir des matrices projection et vue
    void calculateFrustum(const glm::mat4& projection, const glm::mat4& view);

    // Vérifie si une boîte englobante est dans le frustum
    bool isInFrustum(const BoundingBox& box) const;
};

#endif // FRUSTUM_H