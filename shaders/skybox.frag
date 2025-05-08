#version 420 core

out vec4 FragColor;

in vec3 texCoords;

uniform samplerCube skyTexture;

void main()
{    
    FragColor = texture(skyTexture, texCoords);
}