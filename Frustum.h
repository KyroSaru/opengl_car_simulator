#ifndef FRUSTUM_H
#define FRUSTUM_H

#include <glm/glm.hpp>
#include "BoundingBox.h"

class Model;

class Frustum {
private:
    // Les 6 plans du frustum
    glm::vec4 planes[6]; 

public:
    // Calcule les plans du frustum � partir des matrices projection et vue
    void calculateFrustum(const glm::mat4& projection, const glm::mat4& view);

    // V�rifie si une bo�te englobante est dans le frustum
    bool isInFrustum(const BoundingBox& box) const;
};

#endif // FRUSTUM_H