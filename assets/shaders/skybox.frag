#version 420 core

// Couleur du fragment
out vec4 FragColor;

// Coord. de textures
in vec3 texCoords;

// Texture de la skybox (jour/nuit)
uniform samplerCube skyTextureDay;
uniform samplerCube skyTextureNight;
// Transition lisse
uniform float blendValue;

void main()
{    
    vec4 dayColor = texture(skyTextureDay, texCoords);
    vec4 nightColor = texture(skyTextureNight, texCoords);
    vec4 skyColor = mix(nightColor, dayColor, blendValue);

    FragColor = skyColor;
}
