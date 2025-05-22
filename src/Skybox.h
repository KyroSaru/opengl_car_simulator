#ifndef SKYBOX_H
#define SKYBOX_H

#include "Shader.h"
#include "Texture.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>

class Skybox
{
public:
    Skybox();

	// Setter du facteur de blend
    void setBlendValue(float value) { blendValue = value; }

    // Dessine la skybox
    void Draw(const glm::mat4& view, const glm::mat4 projection, float time);
private:
    // Tampons de la skybox
    GLuint skyboxVao, skyboxVbo, skyboxEbo;
    // ID des textures
    GLuint dayTextureID, nightTextureID;

    Shader skyboxShader;

	// Faces de la skybox (textures)
    std::string dayFaces[6];
    std::string nightFaces[6];

	// Facteur de blend (transi fluide entre jour et nuit)
    float blendValue = 0.0f;

    // ---------------------------

	// Charge un cubemap à partir de 6 images
    void loadCubeMap(GLuint textureID, const std::string faces[6]);
};

#endif // SKYBOX_H