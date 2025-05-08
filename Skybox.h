#pragma once

#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<vector>
#include<string>
#include"Camera.h"
#include"Shader.h"
#include"Texture.h"

class Skybox
{
public:
    Skybox();

    void draw(const glm::mat4& view, const glm::mat4 projection);
private:
    GLuint skyboxVao, skyboxVbo, skyboxEbo;
    GLuint texture;
    std::string faces[6];
    Shader shader;
};